/**@file
 * Gamma board interface
 * supports putting or removing pawns, areas management, and printing board
 */

#include <stdint.h>
#include "stdbool.h"
/**
 * board struct typedef
 */
typedef struct board* board_t;
/**
 * @name Init and delete
 */
//@{
/**
 * Creates new Board
 * @param[in] width
 * @param[in] height
 * @return new board with all player fields set to 0
 */
board_t create_board(uint32_t width, uint32_t height);

/**
 * @brief Free memory allocated for board struct
 * @param[in,out] b board to delete
 */
void delete_board(board_t b);
//@}

/**
 *@name Fields management
 */
//@{

/**
 *
 * @param[in] b Board object
 * @param[in] x
 * @param[in] y
 * @return player number, 0 if given field wasn't taken
 */
uint64_t get_player(board_t b, uint32_t x, uint32_t y);

/**
 * @brief Set field at coords (x,y) to player number
 * @param[out] b Board object
 * @param[in] player
 * @param[in] x
 * @param[in] y
 */
void set_player(board_t b, uint64_t player, uint32_t x, uint32_t y);

/**
 * @brief Outputs information about neighbours to output arrays. Arrays have to be allocated before,
 * with length of at least 4
 * @param[in] b Board object
 * @param[in] x
 * @param[in] y
 * @param[out] neighbours_x_array
 * @param[out] neighbours_y_array
 * @param[out] neighbours_player_array
 * @param[out] neighbour_exists if neighbour_exists[i] is false then data in other arrays
 *                              on index 'i' is undefined,
 *
 */
void get_neighbours(board_t b, uint32_t x, uint32_t y,
                    uint32_t neighbours_x_array[],
                    uint32_t neighbours_y_array[],
                    uint32_t neighbours_player_array[],
                    bool neighbour_exists[]);
//@}

/**
 * @name Area management
 */
//@{
/**
 * checks if removing vertex splits areas
 * @param b
 * @param x
 * @param y
 * @return
 */
bool is_cut_vertex(board_t b, uint32_t x, uint32_t y);

void calculate_cut_vertices(board_t b);

void reset_cut_vertices(board_t b);

/**
 * Checks if field at (x,y) was already added to area
 * @param[in] b
 * @param[in] x
 * @param[in] y
 * @return
 */
bool was_visited_in_dfs(board_t b, uint32_t x, uint32_t y);

/**
 * @brief Clean structure of areas on the board
 * @param[in,out] b
 */
void reset_all_areas(board_t b);

/**
 * @brief merge 2 areas with given representatives
 * @param[in,out] b
 * @param[in] x1
 * @param[in] y1
 * @param[in] x2
 * @param[in] y2
 * @return
 */
bool union_operation(board_t b, uint32_t x1, uint32_t y1, uint32_t x2, __uint32_t y2);
//@}

/**
 * @brief recursively attaches to area all fields adjacent to (x,y)
 * @param[in,out] b
 * @param[in] x
 * @param[in] y
 */
void create_area(board_t b, uint32_t x, uint32_t y);
//@}

/**
 * @brief create buffer with board state
 * @param[in] b
 * @return board state
 */
char* get_board(board_t b);
