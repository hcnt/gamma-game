#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * structure to store information about player
 */
typedef struct player {
    uint32_t player_index; ///< player unique number
    uint32_t areas; ///< number of areas this player currently has
    uint64_t pawns_number; ///< number of pawns this player currently has
    bool golden_move_available; ///< flag to store if golden move was already made by this player
    uint64_t area_edges; ///< number of empty fields that are adjacent with fields taken by player
} player;

#endif //GAMMA_PLAYER_H
