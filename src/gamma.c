#include "gamma.h"
#include "stdlib.h"
#include "node.h"
#include "player.h"
#include "gamma_struct.h"

uint64_t get_number_of_players_area_edges(gamma_t* b, uint32_t player);

static bool areParametersForNewGameValid(uint32_t width, uint32_t height,
                                         uint32_t players) {
    if (width <= 0) {
        return false;
    }
    if (height <= 0) {
        return false;
    }
    if (players <= 0) {
        return false;
    }
    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (!areParametersForNewGameValid(width, height, players)) {
        return NULL;
    }
    return create_board(width,height,players,areas);
}

void gamma_delete(gamma_t* g) {
    if (g != NULL) {
        delete_board(g);
    }
}

static bool are_gamma_move_parameters_valid(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL) {
        return false;
    }
    if (player > g->number_of_players) {
        return false;
    }
    if (player == 0) {
        return false;
    }
    if (x > g->width) {
        return false;
    }
    if (y > g->height) {
        return false;
    }
    return true;
}

bool gamma_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!are_gamma_move_parameters_valid(g, player, x, y)) {
        return false;
    }
    if (check_field(g, x, y) != 0) {
        return false;
    }
    bool added_to_area_flag = number_of_neighbours_taken_by_player(g, player, x, y);
    if (get_number_of_players_areas(g, player) == g->max_areas && !added_to_area_flag) {
        return false;
    }
    add_pawn(g, player, x, y);
    return true;
}

bool gamma_golden_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    //---PARAMETER CHECK----
    if (g == NULL) return false;
    if (player > g->number_of_players || player == 0) {
        return false;
    }
    if (!g->players[player-1]->golden_move_available) {
        return false;
    }
    //----------------------

    node node_to_remove = get(g->node_tree, x, y);
    if(node_to_remove == NULL || node_to_remove->player->player_index == player-1){
        return false;
    }
    uint32_t player_to_remove_index = node_to_remove->player->player_index;
    remove_pawn(g, x, y);
    update_areas(g);
    if (get_number_of_players_areas(g, player_to_remove_index+1) > g->max_areas) {
        add_pawn(g, player_to_remove_index+1, x, y);
        return false;
    }
    add_pawn(g, player, x, y);
    if (get_number_of_players_areas(g, player) > g->max_areas) {
        remove_pawn(g, x, y);
        update_areas(g);
        add_pawn(g, player_to_remove_index+1, x, y);
        return false;
    }
    return true;
}

uint64_t gamma_busy_fields(gamma_t* g, uint32_t player) {
    if (g == NULL) return 0;
    if (player > g->number_of_players || player == 0) {
        return 0;
    }
    return get_number_of_players_pawns(g, player);
}

bool gamma_golden_possible(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return false;
    if (player > g->number_of_players || player == 0) {
        return false;
    }
    if (!g->players[player-1]->golden_move_available) return false;
    if (get_number_of_pawns(g) - get_number_of_players_pawns(g, player) == 0) {
        return false;
    }
}

uint64_t gamma_free_fields(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return 0;
    if (player > g->number_of_players || player == 0) {
        return 0;
    }
    if(g->players[player-1]->areas == g->max_areas) {
        return get_number_of_players_area_edges(g, player);
    }
    return g->width * g->height - get_number_of_pawns(g);
}

char* gamma_board(gamma_t* g){
    return print_board(g);
}
