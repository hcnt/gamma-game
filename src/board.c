#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"

struct board {
    uint32_t width;
    uint32_t height;
    uint32_t* players;
    bool* dfs_visited;
    uint32_t* funion_parent;
};

board_t create_board(uint32_t width, uint32_t height) {
    board_t board = malloc(sizeof(struct board));
    board->width = width;
    board->height = height;
    uint64_t array_length = height * width;
    board->players = calloc(array_length, sizeof(uint64_t));
    board->dfs_visited = calloc(array_length, sizeof(bool));
    board->funion_parent = calloc(array_length, sizeof(uint64_t));
    for(uint64_t i = 0; i < array_length; i++){
        board->funion_parent[i] = i;
    }
    return board;
}

void delete_board(board_t b) {
    free(b->funion_parent);
    free(b->players);
    free(b->dfs_visited);
    free(b);
}

uint64_t get_player(board_t b, uint32_t x, uint32_t y) {
    if (b->width * y + x >= b->height * b->width) {
        return 0;
    }
    return b->players[b->width * y + x];
}
void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y) {
    b->players[b->width * y + x] = player;
}

bool get_dfs_visited(board_t b, uint32_t x, uint32_t y) {
    return b->dfs_visited[b->width * y + x];
}

void set_dfs_visited(board_t b, bool dfs_visited, uint32_t x, uint32_t y) {
    b->dfs_visited[b->width * y + x] = dfs_visited;
}
void reset_funion_parent(board_t b, uint32_t x, uint32_t y){
    b->funion_parent[b->width * y + x] = b->width * y + x;
}

void get_neighbours(board_t b, uint32_t x, uint32_t y,
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
//--------FUNION FUNCTIONS --------------

static uint64_t find_root(board_t b, uint64_t element_index) {
    uint64_t root = element_index;
    while (b->funion_parent[root] != root) {
        root = b->funion_parent[root];
    }
    uint64_t tmp = element_index;
    uint64_t parent;
    while (b->funion_parent[tmp] != tmp) {
        parent = b->funion_parent[tmp];
        b->funion_parent[tmp] = root;
        tmp = parent;
    }
    return root;
}

bool union_operation(board_t b, uint32_t x1, uint32_t y1, uint32_t x2, __uint32_t y2) {
    if (get_player(b, x1, y1) == 0 || get_player(b, x2, y2) == 0) {
        return false;
    }
    uint64_t index_1 = b->width * y1 + x1;
    uint64_t index_2 = b->width * y2 + x2;
    uint64_t root1 = find_root(b, index_1);
    uint64_t root2 = find_root(b, index_2);
    if (root1 == root2) {
        return false;
    }
    b->funion_parent[root1] = root2;
    return true;
}

//------------DFS and areas--------------
void reset_dfs_visited_flag(board_t b) {
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            set_dfs_visited(b, false, i, j);
        }
    }
}
void reset_funion_parents(board_t b) {
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            reset_funion_parent(b, i, j);
        }
    }
}

void create_area(board_t b, uint32_t x, uint32_t y, uint32_t new_root_x, uint32_t new_root_y) {
    if (get_player(b, x, y) == 0)
        return;
    if (get_dfs_visited(b, x, y))
        return;
    set_dfs_visited(b, true, x, y);
    union_operation(b, x, y, new_root_x, new_root_y);

    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && get_player(b, x, y) == neighbours_player[i] &&
            !get_dfs_visited(b, neighbours_x[i], neighbours_y[i])) {
            create_area(b, neighbours_x[i], neighbours_y[i], new_root_x, new_root_y);
        }
    }
}

//recursively attaches all neighbouring player fields to area with field at (x,y), and sets all
//these fields visited status to true
//void create_area(board_t b, uint32_t x, uint32_t y) {
//    create_area_r(b,x,y,x,y);
//}
//------------PRINTING-------------------------------------

static void fill_cell(board_t b, char* buffer, int x, int y) {
    uint32_t player = get_player(b, x, b->height - y - 1);
    if (player != 0) {
        buffer[y * (b->width + 1) + x] = (char) (player + '0');
    } else {
        buffer[y * (b->width + 1) + x] = '.';
    }
}

void fill_buffer(board_t b, char* buffer) {
    for (uint32_t i = 0; i < b->height; i++) {
        for (uint32_t j = 0; j < b->width; j++) {
            fill_cell(b, buffer, j, i);
        }
        buffer[i * (b->width + 1) + b->width] = '\n';
    }

}
