#include <ncurses.h>
#include "state.h"


void run_interactive_mode(State state) {
    initscr();
    cbreak();
    noecho();
    clear();

    WINDOW * game = newwin(state->gamma_h, state->gamma_w, state->gamma_h, 0);
    wrefresh(game);
}
