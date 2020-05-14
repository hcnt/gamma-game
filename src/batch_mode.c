#include "gamma_utils.h"
#include "batch_mode.h"
#include <stdlib.h>
#include <ctype.h>

static void handleMove(State state) {
    char* stringLeftToParse;
    bool errorFlag = false;

    if (state->num_chars == 1 || !isspace(state->buffer[1])) {
        error(state->line);
        return;
    }
    uint32_t player = parse32bitInt(state->buffer + 1, &stringLeftToParse, &errorFlag);
    uint32_t x = parse32bitInt(stringLeftToParse, &stringLeftToParse, &errorFlag);
    uint32_t y = parse32bitInt(stringLeftToParse, &stringLeftToParse, &errorFlag);
    if (errorFlag || !checkIfStringHasOnlyWhiteChars(stringLeftToParse)) {
        error(state->line);
        return;
    }
    if (state->buffer[0] == 'm') {
        printf("%d\n", gamma_move(state->gamma, player, x, y));
    } else {
        printf("%d\n", gamma_golden_move(state->gamma, player, x, y));
    }
}

static void handlePlayerProperty(State state) {
    if (state->num_chars == 1 || !isspace(state->buffer[1])) {
        error(state->line);
        return;
    }
    char* stringLeftToParse;
    bool errorFlag = false;
    uint32_t player = parse32bitInt(state->buffer + 2, &stringLeftToParse, &errorFlag);
    if (errorFlag || !checkIfStringHasOnlyWhiteChars(stringLeftToParse)) {
        error(state->line);
        return;
    }
    switch (state->buffer[0]) {
        case 'b':
            printf("%lu\n", gamma_busy_fields(state->gamma, player));
            break;
        case 'f':
            printf("%lu\n", gamma_free_fields(state->gamma, player));
            break;
        case 'q':
            printf("%d\n", gamma_golden_possible(state->gamma, player));
            break;
    }
}

static void handleBoardPrint(State state) {
    if (!checkIfStringHasOnlyWhiteChars(state->buffer + 1)) {
        error(state->line);
        return;
    }
    char* board = gamma_board(state->gamma);
    if (!board) {
        error(state->line);
        return;
    }
    printf("%s", board);
    free(board);
}

void parseLineInBatchMode(State state) {
    char op = state->buffer[0];
    if (op == 'm' || op == 'g') {
        handleMove(state);
    } else if (op == 'b' || op == 'f' || op == 'q') {
        handlePlayerProperty(state);
    } else if (op == 'p') {
        handleBoardPrint(state);
    } else {
        error(state->line);
        return;
    };
}
