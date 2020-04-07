#include "gamma_struct.h"


static void get_neighbours(board_t b, uint32_t x, uint32_t y,
                           uint32_t neighbours_x_array[],
                           uint32_t neighbours_y_array[],
                           uint32_t neighbours_player_array[],
                           bool neighbour_exists[]) {
    uint32_t neighbour_x;
    uint32_t neighbour_y;

    for (int i = 0; i < 4; i++) {
        if ((x == 0 && i == 2) || (x == b->width - 1 && i == 0) || (y == 0 && i == 3) ||
            (y == b->height - 1 && i == 1)) {
            neighbour_exists[i] = false;
        } else {
            neighbour_exists[i] = true;
        }
        neighbour_x = x + (i == 0) - (i == 2);
        neighbour_y = y + (i == 1) - (i == 3);
        neighbours_x_array[i] = neighbour_x;
        neighbours_y_array[i] = neighbour_y;
        neighbours_player_array[i] = get_player(b, neighbour_x, neighbour_y);
    }
}

static void merge(gamma_t* g, uint32_t x, uint32_t y) {

    int merged_areas = 0;
    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(g->b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && neighbours_player[i] == get_player(g->b, x, y) &&
            union_operation(g->b, neighbours_x[i], neighbours_y[i], x, y)) {
            merged_areas++;
        }
    }
    g->players[get_player(g->b, x, y) - 1]->areas -= merged_areas - 1;
}

static void update_edges(gamma_t* g, uint32_t player, uint32_t x, uint32_t y, bool addingFlag) {
    int sign = addingFlag ? 1 : -1;
    int number_of_new_edges = 0;
    bool at_least_one_neighbour = false;
    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(g->b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    bool flag = true;
    for (int i = 0; i < 4; i++) {
        if (!neighbours_exists[i])
            continue;

        if (neighbours_player[i] == 0) {
            if (number_of_neighbours_taken_by_player(g, player, neighbours_x[i], neighbours_y[i]) == 1) {
                number_of_new_edges++;
            }
        } else {
            //check if this edge was already added/removed from player
            for (int j = 0; j < i; j++) {
                if (neighbours_exists[j] && neighbours_player[i] == neighbours_player[j]) {
                    flag = false;
                }
            }
            g->players[neighbours_player[i] - 1]->area_edges -= flag * sign;
            flag = true;
        }
    }
    g->players[player - 1]->area_edges += sign * number_of_new_edges;
    g->players[player - 1]->area_edges -= sign * at_least_one_neighbour;
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
        game->players[i]->pawns_number = 0;
        game->players[i]->golden_move_available = true;
    }
    return game;
}

void delete_gamma(gamma_t* g) {
    delete_board(g->b);
    for (uint32_t i = 0; i < g->number_of_players; i++) {
        free(g->players[i]);
    }
    free(g->players);
    free(g);
}

uint32_t check_field_player(gamma_t* g, uint32_t x, uint32_t y) {
    uint32_t player = get_player(g->b, x, y);
    return player;
}

uint32_t get_number_of_players_areas(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->areas;
}

uint64_t get_number_of_players_pawns(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->pawns_number;
}

uint64_t get_number_of_players_area_edges(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->area_edges;
}

uint64_t get_number_of_pawns(gamma_t* g) {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < g->number_of_players; i++) {
        sum += g->players[i]->pawns_number;
    }
    return sum;
}

void add_pawn(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    set_player(g->b, player, x, y);
    set_funion_parent(g->b, x, y, x, y);
    update_edges(g, player, x, y, true);
    merge(g, x, y);
    g->players[player - 1]->pawns_number++;
}

void remove_pawn(gamma_t* g, uint32_t x, uint32_t y) {
    uint32_t player = get_player(g->b, x, y);
    update_edges(g, player, x, y, false);
    g->players[player - 1]->pawns_number--;
    set_player(g->b, 0, x, y);
}

int number_of_neighbours_taken_by_player(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(g->b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    int n = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && neighbours_player[i] != 0 && neighbours_player[i] == player) {
            n++;
        }
    }
    return n;
}

//------------DFS AND UPDATING AREAS---------------------

static void reset_areas(gamma_t* b) {
    for (uint32_t i = 0; i < b->number_of_players; i++) {
        b->players[i]->areas = 0;
    }
}

static void reset_dfs_visited_flag(board_t b) {
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            set_dfs_visited(b, false, i, j);
        }
    }
}

static void update_areas_dfs(board_t b, uint32_t x, uint32_t y, uint32_t new_root_x, uint32_t new_root_y) {
    if (get_player(b, x, y) == 0)
        return;
    if (get_dfs_visited(b, x, y))
        return;
    set_dfs_visited(b, true, x, y);
    union_operation(b, new_root_x, new_root_y, x, y);

    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && get_player(b, x, y) == neighbours_player[i] &&
            !get_dfs_visited(b, neighbours_x[i], neighbours_y[i])) {
            update_areas_dfs(b, neighbours_x[i], neighbours_y[i], new_root_x, new_root_y);
        }
    }
}

static void traverse_and_update_area(gamma_t* g) {
    uint32_t player;
    for (uint32_t i = 0; i < g->b->width; i++) {
        for (uint32_t j = 0; j < g->b->height; j++) {
            player = get_player(g->b, i, j);
            if (player != 0 && !get_dfs_visited(g->b, i, j)) {
                g->players[player - 1]->areas++;
                update_areas_dfs(g->b, i, j, i, j);
            }
        }
    }
}

void update_areas(gamma_t* g) {
    //set all areas to 0
    reset_areas(g);

    //increment area counters by DFS pass through the board
    traverse_and_update_area(g);

    reset_dfs_visited_flag(g->b);
}

//------------PRINTING-------------------------------------

static void fill_cell(board_t b, char* buffer, int x, int y) {
    uint32_t player = get_player(b, x, b->height - y - 1);
    if (player != 0) {
        buffer[y * (b->width + 1) + x] = (char) (player + '0');
    } else {
        buffer[y * (b->width + 1) + x] = '.';
    }
}

static void fill_board(board_t b, char* buffer) {
    for (uint32_t i = 0; i < b->height; i++) {
        for (uint32_t j = 0; j < b->width; j++) {
            fill_cell(b, buffer, j, i);
        }
        buffer[i * (b->width + 1) + b->height] = '\n';
    }

}

char* print_board(gamma_t* g) {

    uint64_t length = (g->height * (g->width + 1) + 1) * sizeof(char);
    char* board = malloc(length);

    fill_board(g->b, board);

    board[length - 1] = '\0';
    return board;
}