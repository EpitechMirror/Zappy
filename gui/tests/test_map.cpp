#include <criterion/criterion.h>
#include "../map/Map.hpp"

Test(Map, default_size_is_zero) {
    Map map;
    cr_assert_eq(map.getWidth(), 0);
    cr_assert_eq(map.getHeight(), 0);
}

Test(Map, set_and_get_size) {
    Map map;
    map.setSize(15, 20);
    cr_assert_eq(map.getWidth(), 15);
    cr_assert_eq(map.getHeight(), 20);
}

Test(Map, debug_message) {
    cr_log_info("Test Map::debug_message is running\n");
    cr_assert(true);
}

Test(Map, set_and_get_tile_resources) {
    Map map;
    map.setSize(3, 3);
    Resources res;
    res.quantities[FOOD] = 5;
    map.setTileResources(1, 2, res);
    cr_assert_eq(map.getTileResources(1, 2).quantities[FOOD], 5);
}

Test(Map, resource_counters) {
    Map map;
    map.setSize(2, 2);
    Resources res;
    res.quantities[FOOD] = 2;
    res.quantities[LINEMATE] = 1;
    map.setTileResources(0, 0, res);
    map.setTileResources(1, 1, res);
    cr_assert_eq(map.getFoodCount(), 4);
    cr_assert_eq(map.getLinemateCount(), 2);
}

Test(Map, eggs_management) {
    Map map;
    map.setSize(2, 2);
    map.addEgg(42, 1, 1);
    cr_assert_eq(map.getEggsCount(), 1);
    cr_assert_eq(map.getEggs()[0].id, 42);
    cr_assert_eq(map.getEggs()[0].x, 1);
    cr_assert_eq(map.getEggs()[0].y, 1);
}

Test(Map, isFullyInitialized) {
    Map map;
    map.setSize(2, 2);
    
    // Set resources for all tiles
    Resources res;
    map.setTileResources(0, 0, res);
    map.setTileResources(0, 1, res);
    map.setTileResources(1, 0, res);
    map.setTileResources(1, 1, res);
    
    // Map should now be fully initialized
    cr_assert(map.isFullyInitialized());
}

Test(Map, set_tile_resources_out_of_bounds) {
    Map map;
    map.setSize(3, 3);
    Resources res;
    res.quantities[FOOD] = 10;
    
    // These shouldn't cause any errors but the values won't be set
    map.setTileResources(-1, 0, res);
    map.setTileResources(0, -1, res);
    map.setTileResources(3, 0, res);
    map.setTileResources(0, 3, res);
    
    // Check that out-of-bounds access returns the default (dummy) resource
    cr_assert_eq(map.getTileResources(-1, 0).quantities[FOOD], 0);
    cr_assert_eq(map.getTileResources(0, -1).quantities[FOOD], 0);
    cr_assert_eq(map.getTileResources(3, 0).quantities[FOOD], 0);
    cr_assert_eq(map.getTileResources(0, 3).quantities[FOOD], 0);
}

Test(Map, additional_resource_counters) {
    Map map;
    map.setSize(2, 2);
    Resources res1, res2;
    
    // Set different resources in different tiles
    res1.quantities[DERAUMERE] = 3;
    res1.quantities[SIBUR] = 2;
    res1.quantities[MENDIANE] = 1;
    map.setTileResources(0, 0, res1);
    
    res2.quantities[PHIRAS] = 4;
    res2.quantities[THYSTAME] = 2;
    map.setTileResources(1, 1, res2);
    
    // Test individual resource counters
    cr_assert_eq(map.getDeraumereCount(), 3);
    cr_assert_eq(map.getSiburCount(), 2);
    cr_assert_eq(map.getMendianeCount(), 1);
    cr_assert_eq(map.getPhirasCount(), 4);
    cr_assert_eq(map.getThystameCount(), 2);
}

Test(Map, remove_nonexistent_egg) {
    Map map;
    map.setSize(2, 2);
    map.addEgg(1, 0, 0);
    
    // Remove an egg that exists
    map.removeEgg(1);
    cr_assert_eq(map.getEggsCount(), 0);
    
    // Try to remove an egg that doesn't exist
    map.removeEgg(999); // This should generate a warning but not crash
    cr_assert_eq(map.getEggsCount(), 0);
}

Test(Map, player_management) {
    Map map;
    map.setSize(5, 5);
    
    // Create and add players
    Player player1(1, {1.0f, 0.0f, 1.0f}, 1, 1, "Team1");
    Player player2(2, {2.0f, 0.0f, 2.0f}, 2, 2, "Team2");
    
    map.addPlayer(player1);
    map.addPlayer(player2);
    
    // Test getting players by ID
    Player* p1 = map.getPlayerById(1);
    Player* p2 = map.getPlayerById(2);
    Player* nonExistent = map.getPlayerById(999);
    
    cr_assert_not_null(p1);
    cr_assert_not_null(p2);
    cr_assert_null(nonExistent);
    
    cr_assert_eq(p1->getId(), 1);
    cr_assert_eq(p2->getId(), 2);
    
    // Test updating player position
    map.updatePlayerPosition(1, {3.0f, 0.0f, 3.0f}, 3);
    cr_assert_float_eq(p1->getPosition().x, 3.0f, 0.001);
    cr_assert_float_eq(p1->getPosition().z, 3.0f, 0.001);
    cr_assert_eq(p1->getOrientation(), 3);
    
    // Test updating player level
    map.updatePlayerLevel(2, 5);
    cr_assert_eq(p2->getLevel(), 5);
    
    // Test updating player inventory
    int inventory[RESOURCE_COUNT] = {5, 4, 3, 2, 1, 0, 0};
    map.updatePlayerInventory(1, inventory);
    cr_assert_eq(p1->getInventory()[FOOD], 5);
    cr_assert_eq(p1->getInventory()[LINEMATE], 4);
    
    // Test getting all players
    const std::vector<Player>& players = map.getPlayers();
    cr_assert_eq(players.size(), 2);
    
    // Test removing a player
    map.removePlayerById(1);
    cr_assert_null(map.getPlayerById(1));
    cr_assert_not_null(map.getPlayerById(2));
    cr_assert_eq(map.getPlayers().size(), 1);
}

Test(Map, incantation_management) {
    Map map;
    map.setSize(5, 5);
    
    // Create players for the incantation
    map.addPlayer(Player(1, {2.0f, 0.0f, 2.0f}, 1, 1, "Team1"));
    map.addPlayer(Player(2, {2.0f, 0.0f, 2.0f}, 1, 1, "Team1"));
    
    // Start an incantation
    std::vector<int> playerIds = {1, 2};
    map.startIncantation(2, 2, 1, playerIds);
    
    // Verify incantation was started
    const std::vector<Incantation>& incantations = map.getActiveIncantations();
    cr_assert_eq(incantations.size(), 1);
    cr_assert_eq(incantations[0].x, 2);
    cr_assert_eq(incantations[0].y, 2);
    cr_assert_eq(incantations[0].level, 1);
    cr_assert_eq(incantations[0].playerIds.size(), 2);
    cr_assert_eq(incantations[0].playerIds[0], 1);
    cr_assert_eq(incantations[0].playerIds[1], 2);
    
    // Start another incantation
    std::vector<int> playerIds2 = {2};
    map.startIncantation(3, 3, 2, playerIds2);
    cr_assert_eq(map.getActiveIncantations().size(), 2);
    
    // Clear one incantation
    map.clearIncantationAt(2, 2);
    cr_assert_eq(map.getActiveIncantations().size(), 1);
    cr_assert_eq(map.getActiveIncantations()[0].x, 3);
    cr_assert_eq(map.getActiveIncantations()[0].y, 3);
    
    // Clear the other incantation
    map.clearIncantationAt(3, 3);
    cr_assert_eq(map.getActiveIncantations().size(), 0);
}

Test(Map, frequency_and_game_over) {
    Map map;
    map.setSize(2, 2);
    
    // Test setting and getting frequency time
    map.setFrequencyTime(100);
    cr_assert_eq(map.getFrequency(), 100);
    
    // Test setting and getting game over status
    cr_assert_not(map._gameOver);
    cr_assert_eq(map._winningTeam, "");
    
    map.setGameOver(true, "TeamA");
    cr_assert(map._gameOver);
    cr_assert_str_eq(map._winningTeam.c_str(), "TeamA");
}

Test(Map, edge_cases) {
    Map map;
    
    // Test with 0x0 map
    map.setSize(0, 0);
    cr_assert_eq(map.getWidth(), 0);
    cr_assert_eq(map.getHeight(), 0);
    
    // Even with 0x0 size, these operations should not crash
    Resources res;
    map.setTileResources(0, 0, res);
    const Resources& result = map.getTileResources(0, 0);
    cr_assert_eq(result.quantities[FOOD], 0);
    
    // Test updating a non-existent player
    map.updatePlayerPosition(999, {1.0f, 0.0f, 1.0f}, 1);
    map.updatePlayerLevel(999, 1);
    int inventory[RESOURCE_COUNT] = {0};
    map.updatePlayerInventory(999, inventory);
    
    // These should all be safe operations that don't crash
    cr_assert_null(map.getPlayerById(999));
    
    // Test clearing a non-existent incantation
    map.clearIncantationAt(999, 999);
    cr_assert_eq(map.getActiveIncantations().size(), 0);
}