#include "gamma.h"
#include "stdlib.h"
#include "node.h"
#include "player.h"
#include "board.h"

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players_number;
    uint32_t areas;
    player* players;
    board_t b;
};

uint64_t get_number_of_players_area_edges(board_t b, uint32_t player);

static player get_player(gamma_t* g, uint32_t i) {
    return g->players[i];
}

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
    gamma_t* game = malloc(sizeof(gamma_t));
    game->b = create_board(width, height, players, areas);
    game->width = width;
    game->height = height;
    game->players_number = players;
    game->areas = areas;
    return game;
}

void gamma_delete(gamma_t* g) {
    if (g != NULL) {
        delete_board(g->b);
        free(g);
    }
}

static bool are_gamma_move_parameters_valid(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL) {
        return false;
    }
    if (player > g->players_number) {
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
    if (check_field(g->b, x, y) != 0) {
        return false;
    }
    bool added_to_area_flag = number_of_neighbours_taken_by_player(g->b, player, x, y);
    if (get_number_of_players_areas(g->b, player) == g->areas && !added_to_area_flag) {
        return false;
    }
    add_pawn(g->b, player, x, y);
    return true;
}

bool gamma_golden_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL) return false;
    if (player > g->players_number || player == 0) {
        return false;
    }
    if (!g->b->players[player-1]->golden_move_available) {
        return false;
    }
    uint32_t player_to_remove_index = find(g->b->node_tree,x,y)->player->player_index;
    remove_pawn(g->b, x, y);
    update_areas(g->b);
    if (get_number_of_players_areas(g->b, player_to_remove_index+1) > g->areas) {
        add_pawn(g->b, player_to_remove_index+1, x, y);
        return false;
    }
    add_pawn(g->b, player, x, y);
    if (get_number_of_players_areas(g->b, player) > g->areas) {
        remove_pawn(g->b, x, y);
        update_areas(g->b);
        add_pawn(g->b, player_to_remove_index+1, x, y);
        return false;
    }
    return true;
}

uint64_t gamma_busy_fields(gamma_t* g, uint32_t player) {
    if (g == NULL) return 0;
    if (player > g->players_number || player == 0) {
        return 0;
    }
    return get_number_of_players_pawns(g->b, player);
}

bool gamma_golden_possible(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return false;
    if (player > g->players_number || player == 0) {
        return false;
    }
    if (!g->b->players[player-1]->golden_move_available) return false;
    if (get_number_of_pawns(g->b) - get_number_of_players_pawns(g->b, player) == 0) {
        return false;
    }
}

uint64_t gamma_free_fields(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return 0;
    if (player > g->players_number || player == 0) {
        return 0;
    }
    if(g->b->players[player-1]->areas == g->b->max_areas) {
        return get_number_of_players_area_edges(g->b, player);
    }
    return g->width * g->height - get_number_of_pawns(g->b);
}

char* gamma_board(gamma_t* g){
    return print_board(g->b);
}
