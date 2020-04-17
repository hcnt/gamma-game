/**@file
 * Implements information about areas using find and union, and creates them using dfs pass
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "vector.h"
#include "board.h"

/**
 * structure to store information about board
 */
struct board {
    uint32_t width; ///< board width
    uint32_t height;///< board height
    uint64_t* fields;///< array to store what player is on given field, 0 if no one
    bool* dfs_visited;///< array of flags to check which fields were already visited during dfs
    uint64_t* funion_parent;///<array to store find & union tree data
    uint8_t* funion_rank; ///< array to store ranks ranks of nodes in find & union
};

//-------------- INIT AND DELETE-----------------
board_t create_board(uint32_t width, uint32_t height) {
    board_t board = malloc(sizeof(struct board));
    board->width = width;
    board->height = height;
    uint64_t array_length = height * width;
    board->fields = calloc(array_length, sizeof(uint64_t));
    if (board->fields == NULL) {
        return NULL;
    }
    board->dfs_visited = calloc(array_length, sizeof(bool));
    if (board->dfs_visited == NULL) {
        free(board->fields);
        return NULL;
    }
    board->funion_rank = calloc(array_length, sizeof(uint64_t));
    if (board->funion_rank == NULL) {
        free(board->fields);
        free(board->dfs_visited);
        return NULL;
    }
    board->funion_parent = calloc(array_length, sizeof(uint64_t));
    if (board->funion_parent == NULL) {
        free(board->fields);
        free(board->dfs_visited);
        free(board->funion_rank);
        return NULL;
    }
    for (uint64_t i = 0; i < array_length; i++) {
        board->funion_parent[i] = i;
    }
    return board;
}

void delete_board(board_t b) {
    free(b->funion_parent);
    free(b->funion_rank);
    free(b->fields);
    free(b->dfs_visited);
    free(b);
}
//-----------------------------------

uint64_t get_player(board_t b, uint32_t x, uint32_t y) {
    if (b->width * y + x >= b->height * b->width) {
        return 0;
    }
    return b->fields[b->width * y + x];
}

void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y) {
    b->fields[b->width * y + x] = player;
}

bool was_added_to_area(board_t b, uint32_t x, uint32_t y) {
    return b->dfs_visited[b->width * y + x];
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


/**
 * find funion root of field
 * @param[in,out] b
 * @param[in] field_index field position in array
 * @return[in] index of root
 */
static uint64_t find_root(board_t b, uint64_t field_index) {
    uint64_t root = field_index;
    while (b->funion_parent[root] != root) {
        root = b->funion_parent[root];
    }
    uint64_t tmp = field_index;
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
    if (b->funion_rank[root1] > b->funion_rank[root2]) {
        b->funion_parent[root2] = root1;
    } else if (b->funion_rank[root1] < b->funion_rank[root2]) {
        b->funion_parent[root1] = root2;
    } else {
        b->funion_parent[root1] = root2;
        b->funion_rank[root2]++;
    }

    return true;
}

//------------DFS and areas--------------

void reset_all_areas(board_t b) {
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            b->dfs_visited[b->width * j + i] = false;
            //set field parent to themselves
            b->funion_parent[b->width * j + i] = b->width * j + i;
            b->funion_rank[b->width * j + i] = 0;
        }
    }
}

void create_area(board_t b, uint32_t x, uint32_t y) {
    if (get_player(b, x, y) == 0)
        return;
    if (was_added_to_area(b, x, y))
        return;
    b->dfs_visited[b->width * y + x] = true;

    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && get_player(b, x, y) == neighbours_player[i] &&
            !was_added_to_area(b, neighbours_x[i], neighbours_y[i])) {
            union_operation(b, x, y, neighbours_x[i], neighbours_y[i]);
            create_area(b, neighbours_x[i], neighbours_y[i]);
        }
    }
}

//------------PRINTING-------------------------------------

/**
 * @brief convert number to chars and add to the end of string
 * @param[in,out] string
 * @param[in] number
 */
static void append_number_to_string(vectorchar* string, uint32_t number) {
    const int buffer_length = 12;
    char buffer[buffer_length];
    for (int i = buffer_length - 1; i >= 0; i--) {
        buffer[i] = (char) (number % 10 + '0');
        number /= 10;
    }
    int i = 0;
    while (i < buffer_length && buffer[i] == '0') i++;
    while (i < buffer_length) {
        append_vectorchar(string, buffer[i]);
        i++;
    }
}

/**
 * put content on field (x,y) to buffer string
 * @param[in] b
 * @param[out] string
 * @param[in] x
 * @param[in] y
 */
static void fill_cell(board_t b, vectorchar* string, int x, int y) {
    uint32_t player = get_player(b, x, b->height - y - 1);
    if (player == 0) {
        append_vectorchar(string, '.');
    } else if (player <= 9) {
        append_vectorchar(string, (char) (player + '0'));
    } else { //if player number is 10 or more, put this number in brackets
        append_vectorchar(string, '[');
        append_number_to_string(string, player);
        append_vectorchar(string, ']');
    }
}

char* get_board(board_t b) {
    vectorchar string = init_vectorchar();
    for (uint32_t i = 0; i < b->height; i++) {
        for (uint32_t j = 0; j < b->width; j++) {
            fill_cell(b, &string, j, i);
        }
        //return NULL if vector couldn't allocate memory
        if (!append_vectorchar(&string, '\n')) {
            return NULL;
        }
    }
    if (!append_vectorchar(&string, '\0')) {
        return NULL;
    }
    return string.array;
}
