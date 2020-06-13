/** @file
 * Lower level Gamma interface.
 */
#ifndef GAMMA_GAMMA_H
#define GAMMA_GAMMA_H

#include "player.h"
#include "board.h"

/**
 * definition of gamma struct
 */
typedef struct gamma gamma_t;


/** @name Init and delete */
///@{
/**
 * Creates new Gamma structure.
 * @param [in] width
 * @param [in] height
 * @param [in] players
 * @param [in] areas
 * @return New gamma structure
 */
gamma_t* create_gamma(uint32_t width, uint32_t height, uint32_t players, uint32_t areas);

/**
 * Deletes gamma structure
 * @param[in] g
 */
void delete_gamma(gamma_t* g);
///@}

/** @name Board operations
 *
 */
///@{

/**
 * @brief put player's pawn at (x,y)
 * @param[in,out] g
 * @param[in] player
 * @param[in] x
 * @param[in] y
 */
void add_pawn(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);

/**
 * @brief remove player's pawn from (x,y)
 * @param[in,out] g Gamma game
 * @param[in] x
 * @param[in] y
 */
void remove_pawn(gamma_t* g, uint32_t x, uint32_t y);

/**
 * @brief rebuilds all data about areas on board
 * @param[in,out] g Gamma game
 */
void update_areas(gamma_t* g);

/**
 * @brief get buffer with string represenation of the board
 * @param[in] g
 * @return
 */

char* print_board(gamma_t* g);
///@}

/** @name Accessors
 *
 */
///@{

/**
 * get player number from field(x,y)
 * @param[in] g
 * @param[in] x
 * @param[in] y
 * @return player number, 0 if field empty
 */
uint32_t get_player_at_position(gamma_t* g, uint32_t x, uint32_t y);

/**
 * gets number of players in a Gamma game
 * @param[in] g
 * @return
 */
uint32_t get_number_of_players(gamma_t* g);

/**
 * gets gamma board width
 * @param[in] g
 * @return
 */
uint32_t get_width(gamma_t* g);

/**
 * gets gamma board height
 * @param[in] g
 * @return
 */
uint32_t get_height(gamma_t* g);


/**
 * gets maximum number of areas a player can have
 * @param[in] g
 * @return
 */
uint32_t get_max_areas(gamma_t* g);

/**
 * set players golden move to be done
 * @param[in] g
 * @param[in] player
 * @return
 */
void set_player_done_golden_move(gamma_t* g, uint32_t player);

/**
 * check if player as already done golden move
 * @param[in] g
 * @param[in] player
 * @return `true` if player done golden move in this game, `false` otherwise
 */
bool has_player_done_golden_move(gamma_t* g, uint32_t player);

/**
 * gets number of all taken fields on board
 * @param[in] g
 * @return
 */
uint64_t get_number_of_pawns(gamma_t* g);

/**
 * gets number of all free fields on board
 * @param[in] g
 * @return
 */
uint64_t get_number_of_free_fields(gamma_t* g);

/**
 * gets number of fields taken by player
 * @param[in] g
 * @param player
 * @return
 */
uint64_t get_number_of_players_pawns(gamma_t* g, uint32_t player);

/**
 * gets number of areas that player has
 * @param[in] g
 * @param[in] player
 * @return
 */
uint32_t get_number_of_players_areas(gamma_t* g, uint32_t player);

/**
 * gets number of free fields adjacent to players fields
 * @param[in] g
 * @param[in] player
 * @return
 */
uint64_t get_number_of_players_area_edges(gamma_t* g, uint32_t player);

/**
 * get number of adjacent fields to (x,y) that have the same player as (x,y)
 * @param[in] g
 * @param[in] player
 * @param[in] x
 * @param[in] y
 * @return number of adjacent fields, 0 if (x,y) isn't taken
 */
int number_of_neighbours_taken_by_player(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);
///@}

#endif //GAMMA_GAMMA_H
