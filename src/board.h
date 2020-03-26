#ifndef GAMMA_BOARD_H
#define GAMMA_BOARD_H

#include "stdint.h"
#include "stdbool.h"

typedef struct board* board_t;

board_t create_board(uint32_t width, uint32_t height,
                     uint32_t players, uint32_t areas);

void delete_board(board_t b);

void add_pawn(board_t b, uint32_t x, uint32_t y);

void remove_pawn(board_t b, uint32_t x, uint32_t y);

//returns player number, 0 if is empty
uint32_t check_field(board_t b, uint32_t x, uint32_t y);

uint64_t get_number_of_pawns(board_t b);

uint64_t get_number_of_players_pawns(board_t b, uint32_t player);

uint32_t get_number_of_players_areas(board_t b, uint32_t player);

uint64_t get_number_of_free_fields(board_t b);

uint32_t get_number_of_border_fields(board_t b, uint32_t player);

bool check_if_any_neighbour_is_taken_by_player(board_t b, uint32_t player, uint32_t x, uint32_t y);




#endif //GAMMA_BOARD_H
