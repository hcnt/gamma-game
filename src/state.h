#ifndef GAMMA_STATE_H
#define GAMMA_STATE_H

#include <stdio.h>
#include "gamma_backend/gamma.h"

enum state_mode {
    MODE_CHOOSE,
    BATCH_MODE,
};

struct state {
    gamma_t* gamma;
    int gamma_h;
    int gamma_w;
    enum state_mode mode;
    int line;
    size_t buffer_size;
    char* buffer;
    bool eof;
};
typedef struct state* State;

#endif //GAMMA_STATE_H
