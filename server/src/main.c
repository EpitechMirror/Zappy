#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>

#include <stdio.h>

void print_help()
{
    printf(
        "USAGE: ./zappy_server -p port -x width -y height "
        "-n name1 name2 ... -c clientsNb -f freq\n"
    );
    printf("\n");
    printf("DESCRIPTION:\n");
    printf("  -p port        is the port number\n");
    printf("  -x width       is the width of the world\n");
    printf("  -y height      is the height of the world\n");
    printf("  -n name1 name2 ... is the name of the teams\n");
    printf("  -c clientsNb   is the number of authorized clients per team\n");
    printf("  -f freq        is the reciprocal of time unit for execution "
           "of actions\n");
}



int innit_server()
{
    int server_fd;
    int new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t arrlen = sizeof(address);
    char buffer[1024] = {0};
    char hello[] = "hello world!";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
    };
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("set sock pot\n");
        exit(84);
    };
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))){
        perror("bind failed");
        exit(84);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  &arrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024-1);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    close(new_socket);
    close(server_fd);
    return 0;
}

int parse(int argc, char **argv)
{

}

int main(int argc, char **argv)
{
    if(argc < 2){
        return 84;
    }
    if (strcmp(argv[1], "-h") || strcmp(argv[1], "-help")){
        print_help();
        return 0;
    }
    parse(argc, argv);
}