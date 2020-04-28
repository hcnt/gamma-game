#include <ncurses.h>
#include <curses.h>
#include "gamma_interactive.h"

bool handleCursorMovement(char key, WINDOW* game) {
    int x = getcurx(game);
    int y = getcury(game);
    if (keyname(key)[1] == 'D') {
        printw("%s", keyname(key));
        endwin();
        return false;
    }
    wrefresh(game);
    switch (key) {
        case 'A':
            y--;
            break;
        case 'B':
            y++;
            break;
        case 'C':
            x++;
            break;
        case 'D':
            x--;
            break;
        default:
            break;
    }
    if (x > 0 && y > 0 && x < game->_maxx && y < game->_maxy) {
        wmove(game, y, x);
    }
    wrefresh(game);
    return true;
}

bool askPlayerForMove(uint32_t player, gamma_t* gamma, WINDOW* game) {
    char ch;
    bool moveMade = false;
    while (!moveMade) {
        ch = getch();
        if (ch == 'G' && gamma_golden_move(gamma, player, game->_curx - 1, game->_cury - 1)) {
            int cursor_x = game->_curx;
            int cursor_y = game->_cury;
            waddch(game, '0' + player);
            wmove(game, cursor_y, cursor_x);
            wrefresh(game);
            moveMade = true;
        } else if (ch == 'M' && gamma_move(gamma, player, game->_curx - 1, game->_cury - 1)) {
            int cursor_x = game->_curx;
            int cursor_y = game->_cury;
            waddch(game, '0' + player);
            wmove(game, cursor_y, cursor_x);
            wrefresh(game);
            moveMade = true;
        } else {
            if (!handleCursorMovement(ch, game)) {
                return false;
            }
        }
    }
    return true;
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

        if (!askPlayerForMove(player, state->gamma, game)) {
            break;
        }
        player = getNextPlayer(player, state->gamma, state->players);
        if (player == -1) {
            break;
        }
    }
    if (!isendwin()) {
        endwin();
    }
    _nc_free_and_exit();
}

