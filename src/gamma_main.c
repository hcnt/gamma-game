#define  _POSIX_C_SOURCE 200809L

#include "gamma_backend/gamma.h"
#include <stdio.h>
#include "gamma_interactive.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "state.h"

typedef struct state* State;

void error(int line) {
    fprintf(stderr, "ERROR %d\n", line);
}

uint32_t parse32bitInt(char* string, char** stringLeftToParse, bool* errorFlag) {
    errno = 0;
    unsigned long n = strtoul(string, stringLeftToParse, 10);
    if (!isspace((*stringLeftToParse)[0])) {
        *errorFlag = true;
        return 0;
    }
    if (errno == EINVAL || errno == ERANGE || string == *stringLeftToParse) {
        *errorFlag = true;
        return 0;
    }
    if (n > UINT32_MAX) {
        *errorFlag = true;
        return 0;
    }
    return n;
}

void opCompleted(int line) {
    printf("OK %d\n", line);
}

bool checkIfContainsMinus(const char* string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '-') {
            return true;
        }
        i++;
    }
    return false;
}

bool checkIfStringHasOnlyWhiteChars(const char* string) {
    int i = 0;
    while (string[i] != '\0') {
        if (!isspace(string[i])) {
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

void handleMove(State state) {
    char* stringLeftToParse;
    bool errorFlag = false;
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

void handlePlayerProperty(State state) {
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

void handleBoardPrint(State state) {
    if (!checkIfStringHasOnlyWhiteChars(state->buffer + 1)) {
        error(state->line);
        return;
    }
    char* board = gamma_board(state->gamma);
    printf("%s", board);
    free(board);
}

void parseLineWhileChoosingMode(State state) {
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


void parseLineInBatchMode(State state) {
    char op = state->buffer[0];
    if (op == 'm' || op == 'g') {
        handleMove(state);
    } else if (op == 'b' || op == 'f' || op == 'q') {
        if (state->num_chars == 1 || !isspace(state->buffer[1])) {
            error(state->line);
            return;
        }
        handlePlayerProperty(state);
    } else if (op == 'p') {
        handleBoardPrint(state);
    } else {
        error(state->line);
        return;
    };
}

void parseLine(State state) {
    state->num_chars = getline(&(state->buffer), &(state->buffer_size), stdin);
//    printf("%d\n", num_chars);
//    printf("%s", state->buffer);
    if (state->num_chars <= 0) {
        state->eof = true;
        return;
    }
    if (state->buffer[0] == '#' || state->buffer[0] == '\n') {
        state->line++;
        return;
    }
    if (checkIfContainsMinus(state->buffer)) {
        error(state->line);
        state->line++;
        return;
    }
    switch (state->mode) {
        case MODE_CHOOSE:
            parseLineWhileChoosingMode(state);
            break;
        case BATCH_MODE:
            parseLineInBatchMode(state);
            break;
    }
    state->line++;
}

int main() {
    const int BUFFER_START_SIZE = 64;
    State state = malloc(sizeof(struct state));
    state->gamma = NULL;
    state->buffer = malloc(sizeof(char) * BUFFER_START_SIZE);
    state->line = 1;
    state->mode = MODE_CHOOSE;
    state->buffer_size = BUFFER_START_SIZE;
    state->eof = false;

//    state->gamma = gamma_new(5,5,3,5);
//    state->gamma_h = 5;
//    state->gamma_w = 5;
//    state->players = 3;
//    run_interactive_mode(state);
//    gamma_delete(state->gamma);
//    free(state->buffer);
//    free(state);
//    return 0;
    while (!state->eof) {
        parseLine(state);
    }

    gamma_delete(state->gamma);
    free(state->buffer);
    free(state);
}
