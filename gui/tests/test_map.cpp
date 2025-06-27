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
