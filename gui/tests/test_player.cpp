#include <criterion/criterion.h>
#include "../renderer/Player/Player.hpp"

Test(Player, constructor_and_getters) {
    Vector3 pos = {1.0f, 2.0f, 3.0f};
    Player player(42, pos, 2, 5, "redteam");
    cr_assert_eq(player.getId(), 42);
    cr_assert_eq(player.getPosition().x, 1.0f);
    cr_assert_eq(player.getPosition().y, 2.0f);
    cr_assert_eq(player.getPosition().z, 3.0f);
    cr_assert_eq(player.getOrientation(), 2);
    cr_assert_eq(player.getLevel(), 5);
    cr_assert_str_eq(player.getTeam().c_str(), "redteam");
}

Test(Player, setters) {
    Vector3 pos = {0, 0, 0};
    Player player(1, pos, 0, 1, "team");
    Vector3 newPos = {4.0f, 5.0f, 6.0f};
    player.setPosition(newPos);
    player.setOrientation(3);
    player.setLevel(7);
    cr_assert_eq(player.getPosition().x, 4.0f);
    cr_assert_eq(player.getOrientation(), 3);
    cr_assert_eq(player.getLevel(), 7);
}

Test(Player, static_team_names) {
    Player::addTeamName("blue");
    Player::addTeamName("red");
    const auto& teams = Player::getTeamNames();
    cr_assert(teams.size() >= 2);
    cr_assert_str_eq(teams[0].c_str(), "blue");
    cr_assert_str_eq(teams[1].c_str(), "red");
}
