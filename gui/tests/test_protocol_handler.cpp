/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ProtocolHandler unit tests
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../client/ProtocolHandler.hpp"
#include "../map/Map.hpp"
#include "../player/Player.hpp"

// Redirection des sorties console pour les tests
void redirect_outputs(void) {
    cr_redirect_stderr();
    cr_redirect_stdout();
}

// Fonction utilitaire pour créer une commande bct
std::string make_bct_command(int x, int y, const int resources[7]) {
    std::string cmd = "bct " + std::to_string(x) + " " + std::to_string(y);
    for (int i = 0; i < 7; i++) {
        cmd += " " + std::to_string(resources[i]);
    }
    return cmd;
}

// Tests de base
Test(protocol_handler, basic_commands, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Commande inconnue - should not crash
    handler.handleLine("unknown_cmd");
    
    // WELCOME - should not crash
    handler.handleLine("WELCOME");
    
    // Commande vide - should not crash
    handler.handleLine("");
}

// Test de la commande msz
Test(protocol_handler, msz_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Format valide
    handler.handleLine("msz 20 30");
    cr_assert_eq(map.getWidth(), 20);
    cr_assert_eq(map.getHeight(), 30);
    
    // Format invalide - ne devrait pas crasher
    handler.handleLine("msz invalid");
}

// Test de la commande bct
Test(protocol_handler, bct_command, .init = redirect_outputs) {
    Map map;
    map.setSize(10, 10);
    ProtocolHandler handler(map);
    
    int resources[7] = {1, 2, 3, 4, 5, 6, 7};
    handler.handleLine(make_bct_command(2, 3, resources));
    
    const Resources& res = map.getTileResources(2, 3);
    cr_assert_eq(res.quantities[0], 1);
    cr_assert_eq(res.quantities[1], 2);
    cr_assert_eq(res.quantities[2], 3);
    cr_assert_eq(res.quantities[3], 4);
    cr_assert_eq(res.quantities[4], 5);
    cr_assert_eq(res.quantities[5], 6);
    cr_assert_eq(res.quantities[6], 7);
    
    // Format invalide - ne devrait pas crasher et devrait produire un warning
    handler.handleLine("bct x y 1 2 3");
}

// Test de la commande pnw
Test(protocol_handler, pnw_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Format valide
    handler.handleLine("pnw #123 5 6 2 3 teamA");
    const Player* player = map.getPlayerById(123);
    cr_assert_not_null(player);
    cr_assert_eq(player->getPosition().x, 5);
    cr_assert_eq(player->getPosition().z, 6);
    cr_assert_eq(player->getOrientation(), 2);
    cr_assert_eq(player->getLevel(), 3);
    
    // Format invalide - should not crash
    handler.handleLine("pnw invalid");
}

// Test de la commande ppo
Test(protocol_handler, ppo_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 0 team");
    
    // Format valide
    handler.handleLine("ppo #1 3 4 1");
    const Player* player = map.getPlayerById(1);
    cr_assert_eq(player->getPosition().x, 3);
    cr_assert_eq(player->getPosition().z, 4);
    cr_assert_eq(player->getOrientation(), 1);
    
    // Joueur inexistant - should not crash
    handler.handleLine("ppo #999 1 2 3");
}

// Test de la commande sgt
Test(protocol_handler, sgt_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Format valide
    handler.handleLine("sgt 150");
    cr_assert_eq(map.getFrequency(), 150);
    
    // Valeur invalide - should not crash
    handler.handleLine("sgt -5");
}

// Test de la commande pdi
Test(protocol_handler, pdi_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #100 0 0 0 0 team");
    
    // Format valide
    handler.handleLine("pdi #100");
    cr_assert_null(map.getPlayerById(100));
    
    // Format invalide - should not crash
    handler.handleLine("pdi invalid");
}

// Test de la commande seg
Test(protocol_handler, seg_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("seg WinningTeam");
    cr_assert(map._gameOver);
    cr_assert_str_eq(map._winningTeam.c_str(), "WinningTeam");
}

// Test de la commande smg
Test(protocol_handler, smg_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("smg Server message");
    // Should not crash
}

// Test de la commande suc/sbp
Test(protocol_handler, error_commands, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("suc");
    // Should not crash
    
    handler.handleLine("sbp");
    // Should not crash
}

// Test de la commande pgt
Test(protocol_handler, pgt_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    // Initialiser l'inventaire à zéro
    int zeroInv[7] = {0};
    map.getPlayerById(1)->setInventory(zeroInv);
    
    handler.handleLine("pgt #1 0"); // Récupère de la nourriture
    const Player* player = map.getPlayerById(1);
    cr_assert_eq(player->getInventory()[0], 1);
    
    // Ressource invalide - should not crash
    handler.handleLine("pgt #1 10");
}

// Test de la commande pdr
Test(protocol_handler, pdr_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    int inv[7] = {1,0,0,0,0,0,0};
    map.getPlayerById(1)->setInventory(inv);
    
    handler.handleLine("pdr #1 0");
    const Player* player = map.getPlayerById(1);
    cr_assert_eq(player->getInventory()[0], 0);
    
    // Tenter de drop une ressource qu'on n'a pas - should produce warning but not crash
    handler.handleLine("pdr #1 0");
}

// Test de la commande enw
Test(protocol_handler, enw_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("enw #123 #456 5 6");
    const std::vector<Egg>& eggs = map.getEggs();
    cr_assert_eq(eggs.size(), 1);
    cr_assert_eq(eggs[0].id, 123);
    cr_assert_eq(eggs[0].x, 5);
    cr_assert_eq(eggs[0].y, 6);
}

// Test de la commande ebo/edi
Test(protocol_handler, ebo_edi_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("enw #123 #456 5 6");
    cr_assert_eq(map.getEggs().size(), 1);
    
    handler.handleLine("ebo #123");
    cr_assert_eq(map.getEggs().size(), 0);
    
    handler.handleLine("enw #456 #789 7 8");
    handler.handleLine("edi #456");
    cr_assert_eq(map.getEggs().size(), 0);
}

// Test de la commande plv
Test(protocol_handler, plv_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    handler.handleLine("plv #1 2");
    const Player* player = map.getPlayerById(1);
    cr_assert_eq(player->getLevel(), 2);
}

// Test de la commande pic/pie
Test(protocol_handler, incantation_commands, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 teamA");
    handler.handleLine("pnw #2 0 0 0 1 teamA");
    
    // Début d'incantation
    handler.handleLine("pic 5 5 1 #1 #2");
    cr_assert_eq(map.getActiveIncantations().size(), 1);
    const Incantation& inc = map.getActiveIncantations()[0];
    cr_assert_eq(inc.x, 5);
    cr_assert_eq(inc.y, 5);
    cr_assert_eq(inc.level, 1);
    cr_assert_eq(inc.playerIds.size(), 2);
    cr_assert_eq(inc.playerIds[0], 1);
    cr_assert_eq(inc.playerIds[1], 2);
    
    // Fin d'incantation (succès)
    handler.handleLine("pie 5 5 1");
    cr_assert_eq(map.getActiveIncantations().size(), 0);
    
    // Fin d'incantation (échec)
    handler.handleLine("pic 3 3 2 #1");
    handler.handleLine("pie 3 3 0");
}

// Test de la commande pbc
Test(protocol_handler, pbc_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    handler.handleLine("pbc #1 Hello World!");
    // Should not crash
}

// Test d'état du jeu
Test(protocol_handler, game_state) {
    Map map;
    ProtocolHandler handler(map);
    
    cr_assert_not(handler.isMapReady());
    handler.handleLine("msz 10 10");
    
    // Initialiser toute la carte
    int resources[7] = {0};
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            handler.handleLine(make_bct_command(x, y, resources));
        }
    }
    cr_assert(handler.isMapReady());
}

// Test de la commande tna
Test(protocol_handler, tna_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("tna Team1");
    handler.handleLine("tna Team2");
    
    // Vérification indirecte via la création de joueurs
    handler.handleLine("pnw #1 0 0 0 1 Team1");
    handler.handleLine("pnw #2 0 0 0 1 Team2");
    
    const Player* p1 = map.getPlayerById(1);
    const Player* p2 = map.getPlayerById(2);
    
    cr_assert_not_null(p1);
    cr_assert_not_null(p2);
}

// Test de la commande pin
Test(protocol_handler, pin_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    // Format valide
    handler.handleLine("pin #1 0 0 1 2 3 4 5 6 7");
    const Player* player = map.getPlayerById(1);
    cr_assert_not_null(player);
    cr_assert_eq(player->getInventory()[0], 1);
    cr_assert_eq(player->getInventory()[1], 2);
    cr_assert_eq(player->getInventory()[2], 3);
    cr_assert_eq(player->getInventory()[3], 4);
    cr_assert_eq(player->getInventory()[4], 5);
    cr_assert_eq(player->getInventory()[5], 6);
    cr_assert_eq(player->getInventory()[6], 7);
    
    // Format invalide - should not crash
    handler.handleLine("pin #1 0 0 1");
}

// Test de la commande pfk (actuellement commentée dans l'implémentation)
Test(protocol_handler, pfk_command, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    // Appeler la commande même si elle est commentée dans l'implémentation
    handler.handleLine("pfk #1");
}

// Test de cas limite et d'erreur
Test(protocol_handler, edge_cases) {
    Map map;
    ProtocolHandler handler(map);
    
    // Joueur inexistant dans différentes commandes
    handler.handleLine("pgt #999 0");
    handler.handleLine("pdr #999 0");
    handler.handleLine("pbc #999 Message");
    
    // Indices de ressources invalides
    handler.handleLine("pnw #1 0 0 0 1 team");
    handler.handleLine("pgt #1 -1");
    handler.handleLine("pdr #1 -1");
    
    // Format invalide pour pic
    handler.handleLine("pic 5 5");
    handler.handleLine("pic 5 5 1 invalid");
    handler.handleLine("pic 5 5 1");
    
    // Format invalide pour pie
    handler.handleLine("pie 5 5");
    handler.handleLine("pie 5 5 2");
    
    // Format invalide pour pbc
    handler.handleLine("pbc");
    handler.handleLine("pbc #1");
}

// Tests de robustesse avec commandes multiples
Test(protocol_handler, command_sequences) {
    Map map;
    ProtocolHandler handler(map);
    
    // Configuration initiale de la carte
    handler.handleLine("msz 5 5");
    
    // Initialisation des ressources
    int resources[7] = {0};
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            handler.handleLine(make_bct_command(x, y, resources));
        }
    }
    
    // Maintenant la carte devrait être prête
    cr_assert(handler.isMapReady());
    
    // Ajout d'un joueur et test d'actions multiples sur ce joueur
    handler.handleLine("pnw #42 2 2 1 1 TeamX");
    handler.handleLine("pin #42 2 2 0 0 0 0 0 0 0");
    
    // Déplacer le joueur
    handler.handleLine("ppo #42 3 4 2");
    const Player* player = map.getPlayerById(42);
    cr_assert_eq(player->getPosition().x, 3);
    cr_assert_eq(player->getPosition().z, 4);
    cr_assert_eq(player->getOrientation(), 2);
    
    // Monter de niveau et vérifier
    handler.handleLine("plv #42 2");
    cr_assert_eq(player->getLevel(), 2);
    
    // Collecte de ressources
    handler.handleLine("pgt #42 0");
    handler.handleLine("pgt #42 1");
    cr_assert_eq(player->getInventory()[0], 1);
    cr_assert_eq(player->getInventory()[1], 1);
    
    // Déposer une ressource
    handler.handleLine("pdr #42 0");
    cr_assert_eq(player->getInventory()[0], 0);
    
    // Tester une séquence d'incantation
    handler.handleLine("pic 3 4 2 #42");
    cr_assert_eq(map.getActiveIncantations().size(), 1);
    
    // Fin d'incantation
    handler.handleLine("pie 3 4 1");
    cr_assert_eq(map.getActiveIncantations().size(), 0);
    
    // Déconnexion du joueur
    handler.handleLine("pdi #42");
    cr_assert_null(map.getPlayerById(42));
    
    // Fin de partie
    handler.handleLine("seg TeamX");
    cr_assert(map._gameOver);
    cr_assert_str_eq(map._winningTeam.c_str(), "TeamX");
}

// Test comprehensive error handling for string parsing
Test(protocol_handler, string_parsing_errors, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Test malformed player IDs in various commands
    handler.handleLine("pdi malformed_id");  // No # prefix
    handler.handleLine("pdi #");  // Just # without number
    handler.handleLine("pdi #abc");  // Non-numeric after #
    handler.handleLine("pdi #123abc");  // Mixed alphanumeric
    
    // Test malformed IDs in pnw
    handler.handleLine("pnw malformed 0 0 0 1 team");
    handler.handleLine("pnw #abc 0 0 0 1 team");
    
    // Test malformed IDs in ppo
    handler.handleLine("ppo #xyz 1 2 3");
    
    // Test malformed IDs in plv
    handler.handleLine("plv #def 2");
    
    // Test malformed IDs in pin
    handler.handleLine("pin #ghi 0 0 1 2 3 4 5 6 7");
    
    // Test malformed IDs in pgt/pdr
    handler.handleLine("pgt #jkl 0");
    handler.handleLine("pdr #mno 0");
    
    // Test malformed IDs in pbc
    handler.handleLine("pbc #pqr Hello");
    
    // Test malformed egg IDs in enw
    handler.handleLine("enw #abc #def 1 2");
    handler.handleLine("enw malformed #123 1 2");
    
    // Test malformed egg IDs in ebo/edi
    handler.handleLine("ebo #xyz");
    handler.handleLine("edi malformed");
    
    // All these should produce warnings but not crash
}

// Test boundary values and extreme cases
Test(protocol_handler, boundary_values, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Test with maximum and minimum integer values
    handler.handleLine("sgt 2147483647");  // Max int
    handler.handleLine("sgt 0");  // Zero (should produce warning)
    handler.handleLine("sgt -1");  // Negative (should produce warning)
    
    // Test with very large coordinates
    handler.handleLine("msz 10000 10000");
    handler.handleLine("bct 9999 9999 1 2 3 4 5 6 7");
    
    // Test pnw with extreme values
    handler.handleLine("pnw #1 0 0 999 999 team_with_very_long_name");
    
    // Test resource indices at boundaries
    handler.handleLine("pnw #2 0 0 0 1 team");
    int zeroInv[7] = {0};
    if (map.getPlayerById(2)) {
        map.getPlayerById(2)->setInventory(zeroInv);
        handler.handleLine("pgt #2 6");  // Valid max index
        handler.handleLine("pgt #2 7");  // Invalid index (should warn)
        handler.handleLine("pgt #2 -1"); // Invalid negative index
        handler.handleLine("pdr #2 6");  // Valid max index
        handler.handleLine("pdr #2 7");  // Invalid index
    }
}

// Test concurrent operations and state consistency
Test(protocol_handler, state_consistency, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Set up initial state
    handler.handleLine("msz 3 3");
    
    // Add multiple players
    handler.handleLine("pnw #1 0 0 1 1 TeamA");
    handler.handleLine("pnw #2 1 1 2 1 TeamB");
    handler.handleLine("pnw #3 2 2 3 1 TeamA");
    
    // Verify players exist
    cr_assert_not_null(map.getPlayerById(1));
    cr_assert_not_null(map.getPlayerById(2));
    cr_assert_not_null(map.getPlayerById(3));
    
    // Test overlapping operations
    handler.handleLine("pic 1 1 1 #1 #2");  // Start incantation
    handler.handleLine("ppo #1 0 1 2");     // Move player during incantation
    handler.handleLine("pie 1 1 1");        // End incantation
    
    // Test adding and removing eggs
    handler.handleLine("enw #100 #1 1 1");
    handler.handleLine("enw #101 #2 2 2");
    cr_assert_eq(map.getEggs().size(), 2);
    
    handler.handleLine("ebo #100");
    cr_assert_eq(map.getEggs().size(), 1);
    
    handler.handleLine("edi #101");
    cr_assert_eq(map.getEggs().size(), 0);
    
    // Test inventory operations on multiple players
    int inv1[7] = {5, 0, 0, 0, 0, 0, 0};
    int inv2[7] = {0, 5, 0, 0, 0, 0, 0};
    map.getPlayerById(1)->setInventory(inv1);
    map.getPlayerById(2)->setInventory(inv2);
    
    handler.handleLine("pdr #1 0");  // Player 1 drops food
    handler.handleLine("pgt #2 0");  // Player 2 gets food
    
    cr_assert_eq(map.getPlayerById(1)->getInventory()[0], 4);
    cr_assert_eq(map.getPlayerById(2)->getInventory()[0], 1);
    
    // Test disconnection during active operations
    handler.handleLine("pic 0 0 1 #1 #3");  // Start incantation with player 1 and 3
    handler.handleLine("pdi #1");           // Disconnect player 1
    cr_assert_null(map.getPlayerById(1));
    
    // Incantation should still be active (depends on implementation)
    handler.handleLine("pie 0 0 0");        // End incantation with failure
}

// Test malformed command formats
Test(protocol_handler, malformed_commands, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Commands with missing parameters
    handler.handleLine("msz");
    handler.handleLine("msz 10");
    handler.handleLine("bct");
    handler.handleLine("bct 1");
    handler.handleLine("bct 1 2");
    handler.handleLine("bct 1 2 3");  // Missing resources
    
    handler.handleLine("pnw");
    handler.handleLine("pnw #1");
    handler.handleLine("pnw #1 0");
    handler.handleLine("pnw #1 0 0");
    handler.handleLine("pnw #1 0 0 0");
    handler.handleLine("pnw #1 0 0 0 1");  // Missing team name
    
    handler.handleLine("ppo");
    handler.handleLine("ppo #1");
    handler.handleLine("ppo #1 0");
    handler.handleLine("ppo #1 0 0");  // Missing orientation
    
    handler.handleLine("pin");
    handler.handleLine("pin #1");
    handler.handleLine("pin #1 0");
    handler.handleLine("pin #1 0 0");
    handler.handleLine("pin #1 0 0 1");  // Missing inventory items
    
    // Commands with extra parameters (should be handled gracefully)
    handler.handleLine("msz 10 10 extra params");
    handler.handleLine("sgt 100 extra");
    
    // Commands with wrong parameter types
    handler.handleLine("msz abc def");
    handler.handleLine("sgt not_a_number");
    handler.handleLine("bct x y 1 2 3 4 5 6 7");
}

// Test message handling commands
Test(protocol_handler, message_commands, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Test smg with various message formats
    handler.handleLine("smg");  // Empty message
    handler.handleLine("smg Simple message");
    handler.handleLine("smg Message with multiple words and symbols !@#$%");
    handler.handleLine("smg     Message with leading spaces");
    
    // Test pbc with various message formats
    handler.handleLine("pnw #1 0 0 0 1 team");
    handler.handleLine("pbc #1");  // No message
    handler.handleLine("pbc #1 Hello");
    handler.handleLine("pbc #1 Multi word broadcast message");
    handler.handleLine("pbc #1     Broadcast with leading spaces");
    
    // Test seg with various team names
    handler.handleLine("seg TeamA");
    cr_assert(map._gameOver);
    cr_assert_str_eq(map._winningTeam.c_str(), "TeamA");
}

// Test incantation edge cases
Test(protocol_handler, incantation_edge_cases, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // Test pic with various player lists
    handler.handleLine("pnw #1 0 0 0 1 team");
    handler.handleLine("pnw #2 0 0 0 1 team");
    handler.handleLine("pnw #3 0 0 0 1 team");
    
    // Single player incantation
    handler.handleLine("pic 0 0 1 #1");
    cr_assert_eq(map.getActiveIncantations().size(), 1);
    handler.handleLine("pie 0 0 1");
    cr_assert_eq(map.getActiveIncantations().size(), 0);
    
    // Multiple players incantation
    handler.handleLine("pic 1 1 2 #1 #2 #3");
    cr_assert_eq(map.getActiveIncantations().size(), 1);
    const Incantation& inc = map.getActiveIncantations()[0];
    cr_assert_eq(inc.playerIds.size(), 3);
    handler.handleLine("pie 1 1 0");  // Failure
    cr_assert_eq(map.getActiveIncantations().size(), 0);
    
    // pic with invalid player IDs mixed with valid ones
    handler.handleLine("pic 2 2 1 #1 #999 #2 invalid #3");
    // Should still create incantation with valid players only
    
    // pie with invalid result
    handler.handleLine("pic 3 3 1 #1");
    handler.handleLine("pie 3 3 invalid_result");
    
    // pie with missing coordinates
    handler.handleLine("pie");
    handler.handleLine("pie 1");
    handler.handleLine("pie 1 2");  // Missing result
}

// Test resource management edge cases
Test(protocol_handler, resource_management, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    handler.handleLine("pnw #1 0 0 0 1 team");
    
    // Initialize player with some resources
    int initialInv[7] = {10, 5, 3, 2, 1, 0, 0};
    map.getPlayerById(1)->setInventory(initialInv);
    
    // Test dropping resources beyond what player has
    for (int i = 0; i < 15; i++) {
        handler.handleLine("pdr #1 0");  // Drop food 15 times
    }
    
    // Player should have 0 food, and warnings should be generated for excess drops
    cr_assert_eq(map.getPlayerById(1)->getInventory()[0], 0);
    
    // Test getting resources of all types
    for (int resourceType = 0; resourceType < 7; resourceType++) {
        handler.handleLine("pgt #1 " + std::to_string(resourceType));
    }
    
    // Verify inventory was updated
    const int* inv = map.getPlayerById(1)->getInventory();
    for (int i = 0; i < 7; i++) {
        cr_assert_geq(inv[i], 1);  // Should have at least 1 of each resource
    }
    
    // Test resource operations on non-existent player
    handler.handleLine("pgt #999 0");
    handler.handleLine("pdr #999 0");
    
    // Test with invalid resource indices
    handler.handleLine("pgt #1 -5");
    handler.handleLine("pgt #1 100");
    handler.handleLine("pdr #1 -1");
    handler.handleLine("pdr #1 50");
}

// Test output redirection compatibility
Test(protocol_handler, output_compatibility, .init = redirect_outputs) {
    Map map;
    ProtocolHandler handler(map);
    
    // These tests focus on functionality rather than exact output matching
    // to avoid issues with raylib initialization messages
    
    // Test debug messages (should not crash)
    handler.handleLine("WELCOME");
    handler.handleLine("sgt 100");
    
    // Test info messages
    handler.handleLine("smg Test server message");
    handler.handleLine("seg WinningTeam");
    
    // Test warning messages
    handler.handleLine("pdi invalid_format");
    handler.handleLine("pgt #999 0");  // Non-existent player
    handler.handleLine("sgt -1");      // Invalid time
    
    // Test error messages
    handler.handleLine("unknown_command");
    
    // Verify that the protocol handler is still functional after all messages
    handler.handleLine("msz 5 5");
    cr_assert_eq(map.getWidth(), 5);
    cr_assert_eq(map.getHeight(), 5);
    
    handler.handleLine("pnw #1 0 0 0 1 TestTeam");
    cr_assert_not_null(map.getPlayerById(1));
}

// Test command registration and lookup
Test(protocol_handler, command_registration) {
    Map map;
    ProtocolHandler handler(map);
    
    // Test that all expected commands are registered by trying them
    // These should not produce "Unknown command" errors
    
    handler.handleLine("WELCOME");
    handler.handleLine("sgt 100");
    handler.handleLine("msz 10 10");
    handler.handleLine("bct 0 0 1 2 3 4 5 6 7");
    handler.handleLine("tna TeamName");
    handler.handleLine("enw #1 #2 0 0");
    handler.handleLine("ebo #1");
    handler.handleLine("edi #1");
    handler.handleLine("pnw #1 0 0 0 1 team");
    handler.handleLine("ppo #1 1 1 1");
    handler.handleLine("plv #1 2");
    handler.handleLine("pin #1 0 0 1 2 3 4 5 6 7");
    handler.handleLine("pdi #1");
    handler.handleLine("pnw #2 0 0 0 1 team");  // Re-add player for next tests
    handler.handleLine("pgt #2 0");
    handler.handleLine("pdr #2 0");
    handler.handleLine("pfk #2");  // Even if implementation is empty
    handler.handleLine("smg Message");
    handler.handleLine("suc");
    handler.handleLine("sbp");
    handler.handleLine("pic 0 0 1 #2");
    handler.handleLine("pie 0 0 1");
    handler.handleLine("pbc #2 Broadcast");
    handler.handleLine("seg WinningTeam");
    
    // Test truly unknown command
    handler.handleLine("definitely_unknown_command");
    
    // Verify map state is consistent
    cr_assert_eq(map.getWidth(), 10);
    cr_assert_eq(map.getHeight(), 10);
    cr_assert(map._gameOver);
    cr_assert_str_eq(map._winningTeam.c_str(), "WinningTeam");
}