#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include "node.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct player {
    node pawns;
    uint32_t areas;
    uint64_t pawns_number;
} player;

#endif //GAMMA_PLAYER_H
