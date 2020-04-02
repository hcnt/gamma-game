#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"


board_t create_board(uint32_t width, uint32_t height) {
    board_t board = malloc(sizeof(struct board));
    board->width = width;
    board->height = height;
    board->players = calloc(height * width, sizeof(uint64_t));
    board->dfs_visited = calloc(height * width, sizeof(bool));
    board->funion_parent = calloc(height * width, sizeof(uint64_t));
    return board;
}

void delete_board(board_t b) {
    free(b->funion_parent);
    free(b->players);
    free(b->dfs_visited);
    free(b);
}

uint64_t get_player(board_t b, uint32_t x, uint32_t y) {
    if(b->height * y + x < 0 || b->height * y + x >= b->height * b->width){
        return 0;
    }
    return b->players[b->height * y + x];
}

bool get_dfs_visited(board_t b, uint32_t x, uint32_t y) {
    return b->dfs_visited[b->height * y + x];
}

uint64_t get_funion_parent(board_t b, uint32_t x, uint32_t y) {
    return b->funion_parent[b->height * y + x];
}

void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y) {
    b->players[b->height * y + x] = player;
}

void set_dfs_visited(board_t b, bool dfs_visited, uint32_t x, uint32_t y) {
    b->dfs_visited[b->height * y + x] = dfs_visited;
}

void set_funion_parent(board_t b, uint32_t funion_parent_x, uint32_t funion_parent_y, uint32_t x, uint32_t y) {
    b->funion_parent[b->height * y + x] = b->height * funion_parent_y + funion_parent_x;
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
    uint64_t index_1 = b->height * y1 + x1;
    uint64_t index_2 = b->height * y2 + x2;
    uint64_t root1 = find_root(b, index_1);
    uint64_t root2 = find_root(b, index_2);
    if (root1 == root2) {
        return false;
    }
    b->funion_parent[index_1] = root2;
    return true;
}