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
