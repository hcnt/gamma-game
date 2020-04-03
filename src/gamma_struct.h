#ifndef GAMMA_GAMMA_H
#define GAMMA_GAMMA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "player.h"
#include "node.h"

#include "board.h"
// Lower level gamma interface
typedef struct gamma gamma_t;

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t number_of_players;
    uint32_t max_areas;
    player** players;
    board_t b;
};

gamma_t* create_gamma(uint32_t width, uint32_t height,
                      uint32_t players, uint32_t areas);

void delete_gamma(gamma_t* b);

void add_pawn(gamma_t* b, uint32_t player, uint32_t x, uint32_t y);

void remove_pawn(gamma_t* b, uint32_t x, uint32_t y);

//returns player number, 0 if is empty
uint32_t check_field(gamma_t* b, uint32_t x, uint32_t y);

uint64_t get_number_of_pawns(gamma_t* b);

uint64_t get_number_of_players_pawns(gamma_t* b, uint32_t player);

uint32_t get_number_of_players_areas(gamma_t* b, uint32_t player);

uint64_t get_number_of_free_fields(gamma_t* b);

uint32_t get_number_of_border_fields(gamma_t* b, uint32_t player);

int number_of_neighbours_taken_by_player(gamma_t* b, uint32_t player, uint32_t x, uint32_t y);

void update_areas(gamma_t* b);

char* print_board(gamma_t* g);


#endif //GAMMA_GAMMA_H
