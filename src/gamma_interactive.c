#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include "gamma_interactive.h"
#include "gamma_utils.h"
#include <stdlib.h>
#include <assert.h>


//-----------OUTPUT-----------------

static inline void clear() {
    printf("\x1b[2J");
    printf("\x1b[H");
}

static inline int num_digits(uint32_t x) {
    int n = 0;
    while (x > 0) {
        x /= 10;
        n++;
    }
    return n;
}

static void print_board_cell(char* board, uint64_t* i, int padding, long long current_player) {
    int number_length = 0;
    uint32_t player;


    //parse number bigger then 9
    if (board[*i] == '[') {
        player = atoi(&board[*i + 1]);
        if (current_player != -1 && player == current_player) {
            printf("\x1b[45m");
        }
        (*i)++;
        while (board[*i] != ']') {
            printf("%c", board[*i]);
            (*i)++;
            number_length++;
        }
    } else {
        if (current_player != -1 && board[*i] - '0' == current_player) {
            printf("\x1b[45m");
        }
        printf("%c", board[*i]);
        number_length = 1;
    }
    printf("\x1b[0m");

    //apply padding
    for (int j = 0; j < padding - number_length; j++) {
        printf(" ");
    }
}


static void print_state_board(State state, uint32_t cursor_x, uint32_t cursor_y, long long current_player) {
    clear();
    gamma_t* gamma = state->gamma;
    char* board = gamma_board(gamma);
    if (!board) {
        exit(1);
    }
    uint64_t i = 0;
    uint32_t current_x_pos = 0;
    uint32_t current_y_pos = 0;
    while (board[i] != '\0') {

        //go to next line if '\n'
        if (board[i] == '\n') {
            current_y_pos++;
            current_x_pos = 0;
            i++;
            printf("\n");
            continue;
        }

        //color current cursor position
        if (cursor_x == current_x_pos && cursor_y == current_y_pos) {
            printf("\x1b[30;47m");
        }
        print_board_cell(board, &i, num_digits(state->players), current_player);

        printf("\x1b[0m");
        i++;
        current_x_pos++;
    }

    free(board);
}

static void print_state_stats(State state, uint32_t current_player, bool wasMoveFailed) {
    printf("Player: %d\n", current_player);
    printf("Free Fields: %lu\n",
           gamma_free_fields(state->gamma, current_player));
    printf("Golden Possible: %d\n",
           gamma_golden_possible(state->gamma, current_player));
    if (wasMoveFailed) {
        printf("\x1b[30;31m");
        printf("Unable to make that move\n");
        printf("\x1b[0m");
    }
}

static inline void print_game_results(State state) {
    clear();
    printf("Final board state:\n\n");
    print_state_board(state, 0, 0, -1);
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
    return true;
}
//--------------TERMINAL---------------------

static inline void setup_terminal(struct termios* original_terminal) {
    tcgetattr(STDIN_FILENO, original_terminal);
    struct termios term = *original_terminal;
    term.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term)) {
        printf("ERROR\n");
        exit(1);
    }
    printf("\x1b[?25l"); //hide cursor
    clear();
}

static inline void reset_terminal(struct termios* original_terminal) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, original_terminal)) {
        printf("ERROR\n");
        exit(1);
    }
    printf("\x1b[?25h"); //show cursor
    fflush(stdout);
}

//---------------------------------------------

//returns -1 if none of players can make a move
static int get_next_player(long long current_player, gamma_t* gamma, uint32_t number_of_players) {
    uint32_t tmp = current_player;
    for (uint32_t i = 0; i < number_of_players + 1; i++) {
        tmp = ((tmp + i) % number_of_players) + 1;
        if (gamma_free_fields(gamma, tmp) || gamma_golden_possible(gamma, tmp)) {
            return tmp;
        }
    }
    return -1;
}


//returns false if game ended
static bool interactive_loop_step(State state, char c, uint32_t* cursor_x,
                                  uint32_t* cursor_y, long long* player, bool* skipReadFlag) {
    print_state_board(state, *cursor_x, *cursor_y, *player);
    print_state_stats(state, *player, false);
    bool wasMoveFailed = false;

    if (c == ' ' || c == 'G' || c == 'g' || c == 'c' || c == 'C') {
        if (handle_move(c, *player, *cursor_x, state->gamma_h - *cursor_y - 1, state->gamma)) {
            *player = get_next_player(*player, state->gamma, state->players);
            if (*player == -1) {
                return false;
            }
        } else {
            wasMoveFailed = true;
        }
    } else if (c == 27) {
        if (check_for_arrow_input(&c)) {
            handle_cursor_movement(c, cursor_x, cursor_y, state);
        } else {
            *skipReadFlag = true;
        }
    }
    print_state_board(state, *cursor_x, *cursor_y, *player);
    print_state_stats(state, *player, wasMoveFailed);
    return true;

}

void run_interactive_mode(State state) {
    //---------setup------------
    struct termios original_terminal;
    setup_terminal(&original_terminal);

    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;
    long long player = 1;

    bool skipReadFlag = false;
    print_state_board(state, cursor_x, cursor_y, player);
    print_state_stats(state, player, false);

    char c = 1; // init with 0 breaks clion linter, idk why
    //--------------------------------

    //------------game loop-----------
    //if while parsing arrow there is non matching char, e.g sequence "ESC, [ , G"
    //then skipReadFlag is set true and instead of getting next char from stdin,
    //character 'G' would be parsed
    while ((skipReadFlag && c != 4) || ((c = (char) getchar()) && c != 4)) {
        skipReadFlag = false;
        if (!interactive_loop_step(state, c, &cursor_x, &cursor_y, &player, &skipReadFlag)) {
            break;
        }
    }
    //--------------------------------

    //----------end--------------
    print_game_results(state);
    reset_terminal(&original_terminal);
    //------------------------------
}
