typedef struct board* board_t;

#include <stdint.h>
#include "stdbool.h"


bool union_operation(board_t b, uint32_t x1, uint32_t y1, uint32_t x2, __uint32_t y2);

void set_dfs_visited(board_t b, bool dfs_visited, uint32_t x, uint32_t y);

void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y);

void get_neighbours(board_t b, uint32_t x, uint32_t y,
                    uint32_t neighbours_x_array[],
                    uint32_t neighbours_y_array[],
                    uint32_t neighbours_player_array[],
                    bool neighbour_exists[]);

bool get_dfs_visited(board_t b, uint32_t x, uint32_t y);

void reset_funion_parent(board_t b, uint32_t x, uint32_t y);

void reset_funion_parents(board_t b);

uint64_t get_player(board_t b, uint32_t x, uint32_t y);

void reset_dfs_visited_flag(board_t b);

void create_area(board_t b, uint32_t x, uint32_t y, uint32_t new_root_x, uint32_t new_root_y);

void delete_board(board_t b);

board_t create_board(uint32_t width, uint32_t height);

void fill_buffer(board_t b, char* buffer);
