#include "gamma_struct.h"


static void merge(gamma_t* g, uint32_t x, uint32_t y) {
    int merged_areas = 0;
    uint32_t neighbour_x;
    uint32_t neighbour_y;
    set_funion_parent(g->b, x, y, x, y);
    for (int i = 0; i < 4; i++) {
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        if (get_player(g->b, neighbour_x, neighbour_y) == get_player(g->b, x, y) &&
            union_operation(g->b, x, y, neighbour_x, neighbour_y)) {
            merged_areas++;
        }
    }
    g->players[get_player(g->b, x, y) - 1]->areas -= merged_areas - 1;
}

static void update_edges_when_adding(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    int number_of_new_edges = 0;
    bool at_least_one_neighbour = false;
    uint32_t neighbour_x;
    uint32_t neighbour_y;
    uint32_t neighbour_player;
    for (int i = 0; i < 4; i++) {
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        if (neighbour_x < 0 || neighbour_y < 0 || neighbour_x >= g->width ||
            neighbour_y >= g->height) {
            continue;
        }
        neighbour_player = get_player(g->b, neighbour_x, neighbour_y);
        if (neighbour_player != 0) {
            if (neighbour_player == player) {
                at_least_one_neighbour = true;
            } else if (number_of_neighbours_taken_by_player(g, neighbour_player, x, y) == 1) {
                g->players[neighbour_player - 1]->area_edges--;
            }
        } else if (number_of_neighbours_taken_by_player(g, player, neighbour_x, neighbour_y) == 1) {
            number_of_new_edges++;
        }
    }
    g->players[player - 1]->area_edges += number_of_new_edges - at_least_one_neighbour;
}

static void update_edges_when_removing(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    int number_of_new_edges = 0;
    bool at_least_one_neighbour = false;
    uint32_t neighbour_x;
    uint32_t neighbour_y;
    uint32_t neighbour_player;
    for (int i = 0; i < 4; i++) {
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        if (neighbour_x < 0 || neighbour_y < 0 || neighbour_x >= g->width ||
            neighbour_y >= g->height) {
            continue;
        }
        neighbour_player = get_player(g->b, neighbour_x, neighbour_y);
        if (neighbour_player != 0) {
            if (neighbour_player == player) {
                at_least_one_neighbour = true;
            } else if (number_of_neighbours_taken_by_player(g, neighbour_player, x, y) == 1) {
                g->players[neighbour_player - 1]->area_edges++;
            }
        } else if (number_of_neighbours_taken_by_player(g, player, neighbour_x, neighbour_y) == 1) {
            number_of_new_edges++;
        }
    }
    g->players[player - 1]->area_edges -= number_of_new_edges - at_least_one_neighbour;
}
//----------PUBLIC FUNCTIONS------------------------

gamma_t* create_gamma(uint32_t width, uint32_t height,
                      uint32_t players, uint32_t areas) {
    gamma_t* game = malloc(sizeof(struct gamma));
    game->width = width;
    game->height = height;
    game->number_of_players = players;
    game->max_areas = areas;
    game->b = create_board(width, height);
    game->players = malloc(players * sizeof(player*));
    for (uint32_t i = 0; i < players; i++) {
        game->players[i] = malloc(sizeof(player));
        game->players[i]->areas = 0;
        game->players[i]->area_edges = 0;
        game->players[i]->player_index = i;
        game->players[i]->pawns_number = 0;
        game->players[i]->golden_move_available = true;
    }
    return game;
}

void delete_gamma(gamma_t* g) {
    delete_board(g->b);
    free(g);
}

uint32_t check_field(gamma_t* g, uint32_t x, uint32_t y) {
    uint32_t player = get_player(g->b, x, y);
    return player;
}

uint32_t get_number_of_players_areas(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->areas;
}

void add_pawn(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    set_player(g->b, player, x, y);
    update_edges_when_adding(g, player, x, y);
    merge(g, x, y);
    g->players[player - 1]->pawns_number++;
}

void remove_pawn(gamma_t* g, uint32_t x, uint32_t y) {
    uint32_t player = get_player(g->b, x, y);
    update_edges_when_removing(g, player, x, y);
    g->players[player - 1]->pawns_number--;
    set_player(g->b, 0, x, y);
}

uint64_t get_number_of_pawns(gamma_t* b) {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < b->number_of_players; i++) {
        sum += b->players[i]->pawns_number;
    }
    return sum;
}

uint64_t get_number_of_players_pawns(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->pawns_number;
}

int number_of_neighbours_taken_by_player(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbour_x;
    uint32_t neighbour_y;
    uint32_t neighbour_player;
    int n = 0;
    for (int i = 0; i < 4; i++) {
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        neighbour_player = get_player(g->b, neighbour_x, neighbour_y);
        if (neighbour_player != 0 && neighbour_player == player) {
            n++;
        }
    }
    return n;
}

void reset_areas(gamma_t* b) {
    for (uint32_t i = 0; i < b->number_of_players; i++) {
        b->players[i]->areas = 0;
    }
}

void reset_visited(board_t b) {
    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            set_dfs_visited(b, false, i, j);
        }
    }
}

void update_areas_dfs(board_t b, uint32_t x, uint32_t y, uint32_t new_root_x, uint32_t new_root_y) {
    if (get_player(b, x, y) == 0)
        return;
    if (get_dfs_visited(b, x, y))
        return;
    set_dfs_visited(b, true, x, y);
    union_operation(b, new_root_x, new_root_y, x, y);
    uint32_t neighbour_x;
    uint32_t neighbour_y;
    uint32_t neighbour_player;
    for (int i = 0; i < 4; i++) {
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        neighbour_player = get_player(b, neighbour_x, neighbour_y);
        if (get_player(b, x, y) == neighbour_player) {
            update_areas_dfs(b, neighbour_x, neighbour_y, new_root_x, new_root_y);
        }
    }
}

static void traverse_and_update_area(gamma_t* g) {
    for (int i = 0; i < g->b->width; i++) {
        for (int j = 0; j < g->b->height; j++) {
            if (get_player(g->b, i, j) != 0 && !get_dfs_visited(g->b, i, j)) {
                g->players[get_player(g->b, i, j) - 1]->areas++;
                update_areas_dfs(g->b, i, j, i, j);
            }
        }
    }
}

void update_areas(gamma_t* g) {
    reset_areas(g);
    traverse_and_update_area(g);
    reset_visited(g->b);
}


uint64_t get_number_of_players_area_edges(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->area_edges;
}

void fill_board(board_t b, char** buffer) {
    uint32_t player;
    for (int i = 0; i < b->width; i++) {
        for (int j = 0; j < b->height; j++) {
            player = get_player(b, i, j);
            if (player != 0) {
                buffer[j][i] = player + '0';
            }
        }
    }

}

char* print_board(gamma_t* g) {
    char** board = malloc(g->height * sizeof(char*));
    for (uint64_t i = 0; i < g->height; i++) {
        board[i] = malloc((g->width + 1) * sizeof(char));
        for (uint64_t j = 0; j < g->width; j++) {
            board[i][j] = '.';
        }
        board[i][g->width] = '\n';
    }
    fill_board(g->b, board);

    uint64_t length = (g->height * (g->width + 1) + 1) * sizeof(char);
    char* string = malloc(length);
    for (uint64_t i = 0; i < g->height; i++) {
        for (uint64_t j = 0; j < g->width + 1; j++) {
            string[i * (g->width + 1) + j] = board[g->height - 1 - i][j];
        }
    }
    string[length - 1] = '\0';
    return string;
}