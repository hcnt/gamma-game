#include <ncurses.h>
#include "gamma_interactive.h"

void handleCursorMovement(chtype key, WINDOW* game) {
    int x = getcurx(game);
    int y = getcury(game);
    wrefresh(game);
    switch (key) {
        case 2:
            y++;
            break;
        case 3:
            y--;
            break;
        case 4:
            x--;
            break;
        case 5:
            x++;
            break;
        case 113:
            endwin();
        default:
            break;
    }
    if (x > 0 && y > 0 && x < game->_maxx && y < game->_maxy) {
        wmove(game, y, x);
    }
    wrefresh(game);
}

void askPlayerForMove(uint32_t player, gamma_t* gamma, WINDOW* game) {
    char ch;
    while (true) {
        if ((ch = getch()) != ERR) {
            if (ch == 'G' && gamma_golden_move(gamma, player, game->_curx - 1, game->_cury - 1)) {
                waddch(game, '0' + player);
                wrefresh(game);
                break;
            }
            if (ch == 'M' && gamma_move(gamma, player, game->_curx - 1, game->_cury - 1)) {
                waddch(game, '0' + player);
                wrefresh(game);
                break;
            }
            handleCursorMovement(ch, game);
        }
    }


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

void run_interactive_mode(State state) {
    clear();
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    WINDOW* game = newwin((int) state->gamma_h + 2, (int) state->gamma_w + 2, state->gamma_h, 0);
    WINDOW* status = newwin(5, 21, 0, 0);
    wborder(game, 0, 0, 0, 0, 0, 0, 0, 0);
    wborder(status, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove(game, 1, 1);
    refresh();

    int player = 1;
    int cursor_x;
    int cursor_y;
    while (true) {
        cursor_x = getcurx(game);
        cursor_y = getcury(game);

        mvwprintw(status, 1, 1, "Player: %d ", player);
        mvwprintw(status, 2, 1, "Free Fields: %d ", gamma_free_fields(state->gamma, player));
        mvwprintw(status, 3, 1, "Golden Possible: %d ", gamma_golden_possible(state->gamma, player));
        wmove(game, cursor_y, cursor_x);
        wrefresh(status);
        wrefresh(game);

        askPlayerForMove(player, state->gamma, game);
        player = getNextPlayer(player, state->gamma, state->players);
        if (player == -1) {
            break;
        }
    }
    endwin();
}

