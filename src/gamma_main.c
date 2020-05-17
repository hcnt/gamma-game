#define  _POSIX_C_SOURCE 200809L

#include "gamma_backend/gamma.h"
#include "gamma_interactive.h"
#include "batch_mode.h"
#include "gamma_utils.h"
#include <ctype.h>
#include <stdlib.h>

typedef struct state* State;

static inline bool checkIfStringValid(const char* string) {
    int i = 0;
    while (string[i] != '\0') {
        if (!isdigit(string[i]) && !isspace(string[i])) {
            return false;
        }
        i++;
    }
    return true;
}


bool parseGammaArgs(State state) {
    char* stringLeftToParse;
    bool errorFlag = false;
    uint32_t w = parse32bitInt(state->buffer + 2, &stringLeftToParse, &errorFlag);
    uint32_t h = parse32bitInt(stringLeftToParse, &stringLeftToParse, &errorFlag);
    uint32_t players = parse32bitInt(stringLeftToParse, &stringLeftToParse, &errorFlag);
    uint32_t areas = parse32bitInt(stringLeftToParse, &stringLeftToParse, &errorFlag);
    if (errorFlag || !checkIfStringHasOnlyWhiteChars(stringLeftToParse)) {
        return false;
    }
    state->gamma_h = h;
    state->gamma_w = w;
    state->players = players;
    state->gamma = gamma_new(w, h, players, areas);
    return true;
}


void parseLineInStartMode(State state) {
    if (state->buffer[0] != 'I' && state->buffer[0] != 'B') {
        error(state->line);
        return;
    }
    parseGammaArgs(state);
    if (state->gamma == NULL) {
        error(state->line);
        return;
    }
    if (state->buffer[0] == 'B') {
        state->mode = BATCH_MODE;
    }
    opCompleted(state->line);
    if (state->buffer[0] == 'I') {
        run_interactive_mode(state);
        state->eof = true;
    }
}


void parseLine(State state) {
    state->num_chars = getline(&(state->buffer), &(state->buffer_size), stdin);
    if (state->num_chars <= 0) {
        state->eof = true;
        return;
    }

    //check for comment or empty line
    if (state->buffer[0] == '#' || state->buffer[0] == '\n') {
        state->line++;
        return;
    }
    if (!checkIfStringValid(state->buffer + 1)) {
        error(state->line);
        state->line++;
        return;
    }
    switch (state->mode) {
        case START_MODE:
            parseLineInStartMode(state);
            break;
        case BATCH_MODE:
            parseLineInBatchMode(state);
            break;
    }
    state->line++;
}

int main() {
    //--------SETUP---------------
    const int BUFFER_START_SIZE = 64;
    State state = malloc(sizeof(struct state));
    if (!state) {
        exit(1);
    }
    *state = (struct state) {
            .gamma = NULL,
            .buffer = malloc(sizeof(char) * BUFFER_START_SIZE),
            .line = 1,
            .mode = START_MODE,
            .buffer_size = BUFFER_START_SIZE,
            .eof = false
    };
    //--------------------------------


    //-------PARSER LOOP--------------
    while (!state->eof) {
        parseLine(state);
    }
    //--------------------------------


    //---------CLEAN UP---------------
    gamma_delete(state->gamma);
    free(state->buffer);
    free(state);
    //--------------------------------
}
