/**@file
 * implements gamma interface
 */
#include "gamma.h"
#include <stdlib.h>
#include "gamma_struct.h"

/**
 * @brief checks if parameters given to gamma_new are valid
 * @param[in] width
 * @param[in] height
 * @param[in] players
 * @param[in] areas
 * @return true if parameters are valid, false otherwise
 */
static bool are_gamma_new_parameters_valid(uint32_t width, uint32_t height,
                                           uint32_t players,uint32_t areas) {
    return width > 0 && height > 0 && players > 0 && areas > 0;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {

    if (!are_gamma_new_parameters_valid(width, height, players,areas)) {
        return NULL;
    }
    return create_gamma(width, height, players, areas);
}

void gamma_delete(gamma_t* g) {
    if (g != NULL) {
        delete_gamma(g);
    }
}
/**
 * @brief checks if parameters given to gamma_move are valid
 * @param[in] g
 * @param[in] player
 * @param[in] x
 * @param[in] y
 * @return true if parameters are valid, false otherwise
 */
static bool are_gamma_move_parameters_valid(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    return g != NULL && player <= get_number_of_players(g) && player > 0 && x < get_width(g) && y < get_height(g);
}

bool gamma_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!are_gamma_move_parameters_valid(g, player, x, y)) {
        return false;
    }
    //can't move to non empty field
    if (get_player_at_position(g, x, y) != 0) {
        return false;
    }

    //can't add new area when player is already at maximum areas
    bool added_to_area_flag = number_of_neighbours_taken_by_player(g, player, x, y);
    if (get_number_of_players_areas(g, player) == get_max_areas(g) && !added_to_area_flag) {
        return false;
    }

    add_pawn(g, player, x, y);
    return true;
}

bool gamma_golden_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!are_gamma_move_parameters_valid(g, player, x, y)) {
        return false;
    }
    if (has_player_done_golden_move(g, player)) {
        return false;
    }

    //move is not valid if it's done on empty field or players field
    uint32_t player_to_remove = get_player_at_position(g, x, y);
    if (player_to_remove == player || player_to_remove == 0) {
        return false;
    }

    //move is not valid when player has max number of areas, and this move will make another one
    if (get_number_of_players_areas(g, player) == get_max_areas(g) &&
        number_of_neighbours_taken_by_player(g, player, x, y) == 0) {
        return false;
    }

    //make golden move
    remove_pawn(g, x, y);
    add_pawn(g, player, x, y);
    update_areas(g);

    //rollback if not valid
    if (get_number_of_players_areas(g, player) > get_max_areas(g) ||
        get_number_of_players_areas(g, player_to_remove) > get_max_areas(g)) {
        remove_pawn(g, x, y);
        add_pawn(g, player_to_remove, x, y);
        update_areas(g);
        return false;
    }
    set_player_done_golden_move(g, player);
    return true;
}

uint64_t gamma_busy_fields(gamma_t* g, uint32_t player) {
    if (g == NULL || player > get_number_of_players(g) || player == 0) {
        return 0;
    }
    return get_number_of_players_pawns(g, player);
}

bool gamma_golden_possible(gamma_t* g, uint32_t player) {
    //check parameters
    if (g == NULL || player > get_number_of_players(g) || player == 0) {
        return false;
    }
    if (has_player_done_golden_move(g, player)) {
        return false;
    }
    //check if there is at least one pawn that doesn't belong to player
    if (get_number_of_pawns(g) - get_number_of_players_pawns(g, player) == 0) {
        return false;
    }
    return true;
}

uint64_t gamma_free_fields(gamma_t* g, uint32_t player) {
    if (g == NULL || player > get_number_of_players(g) || player == 0) {
        return 0;
    }

    //If player is at maximum number of areas,
    //he can place pawn only at fields neighbouring to already placed fields.
    if (get_number_of_players_areas(g, player) == get_max_areas(g)) {
        return get_number_of_players_area_edges(g, player);
    }
    return get_number_of_free_fields(g);
}

char* gamma_board(gamma_t* g) {
    return print_board(g);
}
