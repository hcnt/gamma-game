/**
 * @file
 * State struct to hold parsing information
 */

#ifndef GAMMA_STATE_H
#define GAMMA_STATE_H

#include <stdio.h>
#include "gamma_backend/gamma.h"

/**
 * Describes current mode
 */
enum state_mode {
    START_MODE, ///while choosing between interactive and batch
    BATCH_MODE, ///batch mode
};

/**
 * Struct describing state
 */
struct state {
    gamma_t* gamma; /// gamma game
    uint32_t gamma_h; /// height of the game
    uint32_t gamma_w; /// width of the game
    uint32_t players; /// number of player in game
    enum state_mode mode; /// current mode
    int line; /// current line
    size_t buffer_size; /// buffer size
    char* buffer; /// buffer for line of text
    int num_chars; ///number of chars in current line
    bool eof; ///if input ended
};
typedef struct state* State;

#endif //GAMMA_STATE_H
