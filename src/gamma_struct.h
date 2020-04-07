/** @file
 * Lower level Gamma interface.
 *
 * @author Jacek Kuśmierczyk
 */
#ifndef GAMMA_GAMMA_H
#define GAMMA_GAMMA_H

#include "player.h"
#include "board.h"

typedef struct gamma gamma_t;
/**
  * Gamma game struct
  * Stores given parameters, array of players and board structure
  */


/** @name Init and delete */
///@{
/**
 * Creates new Gamma structure.
 * @param[in] width
 * @param[in] height
 * @param[in] players
 * @param[in] areas
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
void add_pawn(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);

void remove_pawn(gamma_t* g, uint32_t x, uint32_t y);

void update_areas(gamma_t* g);

char* print_board(gamma_t* g);
///@}

/** @name Accessors
 *
 */
///@{
uint32_t get_player_at_position(gamma_t* g, uint32_t x, uint32_t y);

uint32_t get_number_of_players(gamma_t* g);

uint32_t get_width(gamma_t* g);

uint32_t get_height(gamma_t* g);

uint32_t get_max_areas(gamma_t* g);

void set_player_done_golden_move(gamma_t* g, uint32_t player);

bool has_player_done_golden_move(gamma_t* g, uint32_t player);

uint64_t get_number_of_pawns(gamma_t* g);

uint64_t get_number_of_free_fields(gamma_t* g);

uint64_t get_number_of_players_pawns(gamma_t* g, uint32_t player);

uint32_t get_number_of_players_areas(gamma_t* g, uint32_t player);

uint64_t get_number_of_players_area_edges(gamma_t* g, uint32_t player);

int number_of_neighbours_taken_by_player(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);
///@}

#endif //GAMMA_GAMMA_H
