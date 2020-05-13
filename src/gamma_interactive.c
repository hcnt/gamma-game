#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include "gamma_interactive.h"
#include <stdlib.h>
#include <assert.h>


bool checkforArrowInput(char* c) {
    *c = (char) getchar();
    if (*c != '[') {
        return false;
    }
    *c = (char) getchar();
    if (*c == 'A' || *c == 'B' || *c == 'C' || *c == 'D') {
        return true;
    } else {
        return false;
    }

}

bool handle_cursor_movement(char ch, uint32_t* cursor_x, uint32_t* cursor_y, State state) {
    uint32_t x = *cursor_x;
    uint32_t y = *cursor_y;
    switch (ch) {
        case 'A':
            if (y == 0)
                return true;
            y--;
            break;
        case 'B':
            if (y == state->gamma_h - 1)
                return true;
            y++;
            break;
        case 'C':
            if (x == state->gamma_w - 1)
                return true;
            x++;
            break;
        case 'D':
            if (x == 0)
                return true;
            x--;
            break;
        default:
            assert(false);
    }
    *cursor_x = x;
    *cursor_y = y;
    return true;
}
//
bool handle_move(char ch, uint32_t player, uint32_t x, uint32_t y, gamma_t* gamma) {
    if ((ch == 'G' || ch == 'g') && gamma_golden_move(gamma, player, x, y)) {
        return true;
    } else if (ch == ' ' && gamma_move(gamma, player, x, y)) {
        return true;
    } else if (ch == 'C' || ch == 'c') {
        return true;
    }
    return false;
}

int getNextPlayer(int currentPlayer, gamma_t* gamma, uint32_t numberOfPlayers) {
    uint32_t tmp = currentPlayer;
    for (uint32_t i = 0; i < numberOfPlayers + 1; i++) {
        tmp = ((tmp + i) % numberOfPlayers) + 1;
        if (gamma_free_fields(gamma, tmp) || gamma_golden_possible(gamma, tmp)) {
            return tmp;
        }
    }
    return -1;
}

void clear() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void printBoard(State state, uint32_t current_player, uint32_t cursor_x, uint32_t cursor_y, bool onlyBoard) {
    char* board = gamma_board(state->gamma);
    for (uint32_t i = 0; i < state->gamma_h; i++) {
        for (uint32_t j = 0; j < state->gamma_w; j++) {
            if (!onlyBoard && cursor_x == j && cursor_y == i) {
                printf("\x1b[30;47m");
            }
            printf("%c", board[(state->gamma_w + 1) * i + j]);
            printf("\x1b[0m");
        }
        printf("\n");
    }
    if (!onlyBoard) {
        printf("Player: %d\n", current_player);
        printf("Free Fields: %lu\n", gamma_free_fields(state->gamma, current_player));
        printf("Golden Possible: %d\n", gamma_golden_possible(state->gamma, current_player));
    }
    free(board);
}


void run_interactive_mode(State state) {
    //setup terminal
    struct termios starting_terminal;
    tcgetattr(STDIN_FILENO, &starting_terminal);
    struct termios term = starting_terminal;
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);

    printf("\x1b[?25l"); //hide cursor
    clear();

    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;
    int player = 1;

    bool skipReadFlag = false;
    printBoard(state, player, cursor_x, cursor_y, false);
    char c;
    while (skipReadFlag || ((c = (char) getchar()) && c != 4)) {
        skipReadFlag = false;
        clear();
        printBoard(state, player, cursor_x, cursor_y, false);

        if (c == ' ' || c == 'G' || c == 'g' || c == 'c' || c == 'C') {
            if (handle_move(c, player, cursor_x, state->gamma_h - cursor_y - 1, state->gamma)) {
                player = getNextPlayer(player, state->gamma, state->players);
                if (player == -1) {
                    break;
                }
            }
        } else if (c == 27) {
            if (checkforArrowInput(&c)) {
                handle_cursor_movement(c, &cursor_x, &cursor_y, state);
            } else {
                skipReadFlag = true;
                if (c == 4) {
                    break;
                }
            }
        }
        clear();
        printBoard(state, player, cursor_x, cursor_y, false);
    }

    if (player == -1) {
        clear();
        printf("Final board state:\n\n");
        printBoard(state, player, cursor_x, cursor_y, true);
        printf("\nScore:\n");
        for (uint32_t i = 1; i <= state->players; i++) {
            printf("Player %u: %lu \n", i, gamma_busy_fields(state->gamma, i));
        }
    }

    //clean terminal
//    printf("XDDDDDDD");
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &starting_terminal);
    printf("\x1b[?25h"); //show cursor
    fflush(stdout);
}
