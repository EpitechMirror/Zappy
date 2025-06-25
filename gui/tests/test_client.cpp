#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include "Client.hpp"

// TestableClient pour accéder aux membres privés
class TestableClient : public Client {
public:
    using Client::Client;
    using Client::_buffer;
    using Client::_socket;
    using Client::_protocolHandler;
};

// Serveur factice pour les tests réseau
class MockServer {
public:
    MockServer(int port, const std::string& response) 
        : _port(port), _response(response), _running(false) {}
    
    void start() {
        _running = true;
        _thread = std::thread([this]() { this->run(); });
    }
    
    void stop() {
        _running = false;
        if (_thread.joinable()) _thread.join();
    }
    
    void run() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);
        
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        bind(server_fd, (sockaddr*)&addr, sizeof(addr));
        listen(server_fd, 1);
        
        _running = true;
        int client_fd = accept(server_fd, nullptr, nullptr);
        
        // Attendre la commande GRAPHIC
        char buffer[256];
        recv(client_fd, buffer, sizeof(buffer), 0);
        
        // Envoyer la réponse
        send(client_fd, _response.c_str(), _response.size(), 0);
        
        close(client_fd);
        close(server_fd);
    }

private:
    int _port;
    std::string _response;
    std::thread _thread;
    std::atomic<bool> _running;
};

// Tests unitaires
Test(Client, constructor_and_destructor) {
    TestableClient client("localhost", 4242);
    cr_assert_eq(client._host, "localhost");
    cr_assert_eq(client._port, 4242);
    cr_assert_eq(client._socket, -1);
    cr_assert(client._buffer.empty());
    cr_assert_not(client.isMapReady());
}

Test(Client, connectToServer_invalid_address) {
    TestableClient client("invalid.host", 4242);
    cr_assert_not(client.connectToServer());
    cr_assert_eq(client._socket, -1);
}

Test(Client, sendGraphicCommand_not_connected) {
    TestableClient client("localhost", 4242);
    cr_assert_not(client.sendGraphicCommand());
}

Test(Client, readLine_empty_buffer) {
    TestableClient client("", 0);
    std::string line;
    cr_assert_not(client.readLine(line));
}

Test(Client, readLine_single_line) {
    TestableClient client("", 0);
    client._buffer = "test line\n";
    std::string line;
    cr_assert(client.readLine(line));
    cr_assert_eq(line, "test line");
    cr_assert(client._buffer.empty());
}

Test(Client, readLine_multiple_lines) {
    TestableClient client("", 0);
    client._buffer = "line1\nline2\nline3\n";
    std::string line;
    
    cr_assert(client.readLine(line));
    cr_assert_eq(line, "line1");
    
    cr_assert(client.readLine(line));
    cr_assert_eq(line, "line2");
    
    cr_assert(client.readLine(line));
    cr_assert_eq(line, "line3");
    
    cr_assert(client._buffer.empty());
}

Test(Client, readLine_partial_line) {
    TestableClient client("", 0);
    client._buffer = "partial";
    std::string line;
    
    cr_assert_not(client.readLine(line));
    cr_assert_eq(client._buffer, "partial");
    
    client._buffer += " line\n";
    cr_assert(client.readLine(line));
    cr_assert_eq(line, "partial line");
}

Test(Client, parseData_valid_command, .init = cr_redirect_stderr) {
    TestableClient client("", 0);
    client._buffer = "msz 10 20\n";
    client.parseData();
    
    cr_assert(client.isMapReady());
    cr_assert_eq(client.getMap().getWidth(), 10);
    cr_assert_eq(client.getMap().getHeight(), 20);
}

Test(Client, parseData_unknown_command, .init = cr_redirect_stderr) {
    TestableClient client("", 0);
    client._buffer = "unknown command\n";
    
    cr_assert_not(client.isMapReady());
    client.parseData();
    
    // Vérifie qu'aucune erreur n'a été imprimée
    cr_assert_stderr_eq_str("");
}

Test(Client, receiveData_no_data) {
    TestableClient client("", 0);
    int pipefd[2];
    pipe(pipefd);
    client._socket = pipefd[0];
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    
    client.receiveData();
    cr_assert(client._buffer.empty());
    
    close(pipefd[0]);
    close(pipefd[1]);
}

Test(Client, receiveData_with_data) {
    TestableClient client("", 0);
    int pipefd[2];
    pipe(pipefd);
    client._socket = pipefd[0];
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    
    const char* msg = "test message\n";
    write(pipefd[1], msg, strlen(msg));
    
    client.receiveData();
    cr_assert_eq(client._buffer, "test message\n");
    
    close(pipefd[0]);
    close(pipefd[1]);
}

Test(Client, disconnect_while_connected) {
    TestableClient client("", 0);
    int pipefd[2];
    pipe(pipefd);
    client._socket = pipefd[0];
    
    client.disconnect();
    cr_assert_eq(client._socket, -1);
    
    close(pipefd[1]); // pipefd[0] est déjà fermé par disconnect
}

Test(Client, full_connection_workflow) {
    const int TEST_PORT = 42420;
    MockServer server(TEST_PORT, "msz 15 25\n");
    server.start();
    
    // Laisser le temps au serveur de démarrer
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TestableClient client("127.0.0.1", TEST_PORT);
    
    // Test de connexion
    cr_assert(client.connectToServer());
    cr_assert_neq(client._socket, -1);
    
    // Test d'envoi de commande
    cr_assert(client.sendGraphicCommand());
    
    // Test de réception de données
    for (int i = 0; i < 10; i++) {
        client.update();
        if (client.isMapReady()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    cr_assert(client.isMapReady());
    cr_assert_eq(client.getMap().getWidth(), 15);
    cr_assert_eq(client.getMap().getHeight(), 25);
    
    // Test de déconnexion
    client.disconnect();
    cr_assert_eq(client._socket, -1);
    
    server.stop();
}

Test(Client, getMap_access) {
    TestableClient client("", 0);
    const Map& map = client.getMap();
    cr_assert_eq(map.getWidth(), 0);
    cr_assert_eq(map.getHeight(), 0);
}

Test(Client, isMapReady_initial_state) {
    TestableClient client("", 0);
    cr_assert_not(client.isMapReady());
}