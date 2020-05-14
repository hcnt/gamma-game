#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include "gamma_interactive.h"
#include <stdlib.h>
#include <assert.h>


//-----------OUTPUT-----------------

static inline void clear() {
    printf("\x1b[2J");
    printf("\x1b[H");
}

static void print_state(State state, uint32_t current_player, uint32_t cursor_x, uint32_t cursor_y, bool onlyBoard) {
    clear();
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

static inline void print_game_results(State state) {
    clear();
    printf("Final board state:\n\n");
    print_state(state, 0, 0, 0, true);
    printf("\nScore:\n");
    for (uint32_t i = 1; i <= state->players; i++) {
        printf("Player %u: %lu \n", i, gamma_busy_fields(state->gamma, i));
    }
}

//------------INPUT---------------------
static inline bool check_for_arrow_input(char* c) {
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

static bool handle_cursor_movement(char ch, uint32_t* cursor_x, uint32_t* cursor_y, State state) {
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

static inline bool handle_move(char ch, uint32_t player, uint32_t x, uint32_t y, gamma_t* gamma) {
    if (ch == 'G' || ch == 'g') {
        return gamma_golden_move(gamma, player, x, y);
    }
    if (ch == ' ') {
        return gamma_move(gamma, player, x, y);
    }
    assert(false);
    return true;
}
//--------------TERMINAL---------------------

static inline void setup_terminal(struct termios* original_terminal) {
    tcgetattr(STDIN_FILENO, original_terminal);
    struct termios term = *original_terminal;
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    printf("\x1b[?25l"); //hide cursor
    clear();
}

static inline void reset_terminal(struct termios* original_terminal) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original_terminal);
    printf("\x1b[?25h"); //show cursor
    fflush(stdout);
}

//---------------------------------------------


//returns -1 if none of players can make a move
static int get_next_player(int current_player, gamma_t* gamma, uint32_t number_of_players) {
    uint32_t tmp = current_player;
    for (uint32_t i = 0; i < number_of_players + 1; i++) {
        tmp = ((tmp + i) % number_of_players) + 1;
        if (gamma_free_fields(gamma, tmp) || gamma_golden_possible(gamma, tmp)) {
            return tmp;
        }
    }
    return -1;
}

void run_interactive_mode(State state) {
    //---------setup------------
    struct termios original_terminal;
    setup_terminal(&original_terminal);

    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;
    int player = 1;

    bool skipReadFlag = false;
    print_state(state, player, cursor_x, cursor_y, false);

    char c = 1; // init with 0 breaks clion linter, idk why
    //--------------------------------

    //------------game loop-----------
    while ((skipReadFlag && c != 4) || ((c = (char) getchar()) && c != 4)) {
        skipReadFlag = false;
        print_state(state, player, cursor_x, cursor_y, false);

        if (c == ' ' || c == 'G' || c == 'g' || c == 'c' || c == 'C') {
            if (handle_move(c, player, cursor_x, state->gamma_h - cursor_y - 1, state->gamma)) {
                player = get_next_player(player, state->gamma, state->players);
                if (player == -1) {
                    break;
                }
            }
        } else if (c == 27) {
            if (check_for_arrow_input(&c)) {
                handle_cursor_movement(c, &cursor_x, &cursor_y, state);
            } else {
                skipReadFlag = true;
            }
        }
        print_state(state, player, cursor_x, cursor_y, false);
    }
    //--------------------------------

    //----------end--------------
    if (player == -1) {
        print_game_results(state);
    }
    reset_terminal(&original_terminal);
    //------------------------------
}
