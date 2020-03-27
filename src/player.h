#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct player {
    uint32_t areas;
    uint64_t pawns_number;
    bool golden_move_available;
    uint32_t player_index;
} player;

#endif //GAMMA_PLAYER_H
