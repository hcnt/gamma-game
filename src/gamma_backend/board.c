/**@file
 * Implements information about areas using find and union, and creates them using dfs pass
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "vector.h"
#include "board.h"

/**
 * structure to store information about field
 */
typedef struct {
    uint64_t player;///< what player is on this field, 0 if no one
    bool dfs_visited;///< flag to check if field was already visited during dfs
    uint64_t funion_parent;///<find & union tree parent
    uint8_t funion_rank; ///< rank of node in find & union
    bool is_cut_vertex; ///< stores if field is cut vertex (removing it would split area)
    uint64_t dfs_low;
    uint64_t dfs_parent;

} field;

/**
 * structure to store information about board
 */
struct board {
    uint32_t width; ///< board width
    uint32_t height;///< board height
    field* fields;///< array to store field data
};

//-------------- INIT AND DELETE-----------------
board_t create_board(uint32_t width, uint32_t height) {
    board_t board = malloc(sizeof(struct board));
    if (board == NULL) {
        return NULL;
    }
    board->width = width;
    board->height = height;
    uint64_t array_length = (uint64_t) height * (uint64_t) width;
    board->fields = calloc(array_length, sizeof(field));
    for (uint64_t i = 0; i < array_length; i++) {
        board->fields[i].funion_parent = i;
    }
    return board;
}

void delete_board(board_t b) {
    free(b->fields);
    free(b);
}
//-----------------------------------

uint64_t get_player(board_t b, uint32_t x, uint32_t y) {
    if (b->width * y + x >= b->height * b->width) {
        return 0;
    }
    return b->fields[b->width * y + x].player;
}

void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y) {
    b->fields[b->width * y + x].player = player;
}

bool was_visited_in_dfs(board_t b, uint32_t x, uint32_t y) {
    return b->fields[b->width * y + x].dfs_visited;
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
    while (b->fields[root].funion_parent != root) {
        root = b->fields[root].funion_parent;
    }
    uint64_t tmp = field_index;
    uint64_t parent;
    while (b->fields[tmp].funion_parent != tmp) {
        parent = b->fields[tmp].funion_parent;
        b->fields[tmp].funion_parent = root;
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
    if (b->fields[root1].funion_rank > b->fields[root2].funion_rank) {
        b->fields[root2].funion_parent = root1;
    } else if (b->fields[root1].funion_rank < b->fields[root2].funion_rank) {
        b->fields[root1].funion_parent = root2;
    } else {
        b->fields[root1].funion_parent = root2;
        b->fields[root2].funion_rank++;
    }

    return true;
}

//------------DFS and areas--------------

void reset_all_areas(board_t b) {
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            b->fields[b->width * j + i].dfs_visited = false;
            //set field parent to themselves
            b->fields[b->width * j + i].funion_parent = b->width * j + i;
            b->fields[b->width * j + i].funion_rank = 0;
        }
    }
}

void create_area(board_t b, uint32_t x, uint32_t y) {
    if (get_player(b, x, y) == 0)
        return;
    if (was_visited_in_dfs(b, x, y))
        return;
    b->fields[b->width * y + x].dfs_visited = true;

    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];

    get_neighbours(b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);
    for (int i = 0; i < 4; i++) {
        if (neighbours_exists[i] && get_player(b, x, y) == neighbours_player[i] &&
            !was_visited_in_dfs(b, neighbours_x[i], neighbours_y[i])) {
            union_operation(b, x, y, neighbours_x[i], neighbours_y[i]);
            create_area(b, neighbours_x[i], neighbours_y[i]);
        }
    }
}

//TODO fix this 64bit
static inline void reset_dfs_visited(board_t b) {
    for (uint64_t i = 0; i < (uint64_t) b->width * (uint64_t) b->height; i++) {
        b->fields[i].dfs_visited = false;
    }
}

static void find_cut_vertices(board_t b, uint32_t x, uint32_t y, bool is_root, uint64_t height) {
    uint32_t neighbours_x[4];
    uint32_t neighbours_y[4];
    uint32_t neighbours_player[4];
    bool neighbours_exists[4];
    get_neighbours(b, x, y, neighbours_x, neighbours_y, neighbours_player, neighbours_exists);

    b->fields[b->width * y + x].dfs_visited = true;
    b->fields[b->width * y + x].dfs_low = height;

    int children_num = 0;
    for (int i = 0; i < 4; i++) {
        if (!neighbours_exists[i] || get_player(b, x, y) != neighbours_player[i]) {
            continue;
        }
        if (!was_visited_in_dfs(b, neighbours_x[i], neighbours_y[i])) {
            children_num++;
            b->fields[b->width * neighbours_y[i] + neighbours_x[i]].dfs_parent = b->width * y + x;

            find_cut_vertices(b, neighbours_x[i], neighbours_y[i], false, height + 1);
        } else if (b->fields[b->width * y + x].dfs_parent != b->width * neighbours_y[i] + neighbours_x[i] &&
                   b->fields[b->width * neighbours_y[i] + neighbours_x[i]].dfs_low <
                   b->fields[b->width * y + x].dfs_low) {
            b->fields[b->width * y + x].dfs_low = b->fields[b->width * neighbours_y[i] + neighbours_x[i]].dfs_low;
        }
    }
    if ((is_root && children_num >= 2) ||
        (!is_root && children_num > 0 && b->fields[b->width * y + x].dfs_low == height)) {
        b->fields[b->width * y + x].is_cut_vertex = true;
    }
}

void calculate_cut_vertices(board_t b) {
    reset_dfs_visited(b);
    for (uint32_t i = 0; i < b->width; i++) {
        for (uint32_t j = 0; j < b->height; j++) {
            if (!b->fields[b->width * j + i].dfs_visited &&
                b->fields[b->width * j + i].player != 0) {
                find_cut_vertices(b, i, j, true, 0);
            }
        }
    }
}

bool is_cut_vertex(board_t b, uint32_t x, uint32_t y) {
    return b->fields[b->width * y + x].is_cut_vertex;
}


void reset_cut_vertices(board_t b) {
    for (uint64_t i = 0; i < (uint64_t) b->width * (uint64_t) b->height; i++) {
        b->fields[i].is_cut_vertex = false;
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
