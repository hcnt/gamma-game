#include "gamma_struct.h"


static node* get_neighbours(gamma_t* b, uint32_t x, uint32_t y) {
    node* neighbours = calloc(4, sizeof(node));
    if (x < b->height) {
        neighbours[0] = get(b->node_tree, x + 1, y);
    }
    if (y < b->width) {
        neighbours[1] = get(b->node_tree, x, y + 1);
    }
    if (x > 0) {
        neighbours[2] = get(b->node_tree, x - 1, y);
    }
    if (y > 0) {
        neighbours[3] = get(b->node_tree, x, y - 1);
    }
    return neighbours;
}


static node find_root(node n) {
    node root = n;
    while (root->parent != root) {
        root = root->parent;
    }
    node tmp = n;
    node parent;
    while (tmp->parent != tmp) {
        parent = tmp->parent;
        tmp->parent = root;
        tmp = parent;
    }
    return root;
}

static int merge_neighbours(node n, node new_root) {
    node tmp;
    int merged_areas = 0;
    for (int i = 0; i < 4; i++) {
        if (n->neighbours[i] != NULL && n->neighbours[i]->player->player_index == n->player->player_index) {
            tmp = find_root(n->neighbours[i]);
            if (tmp != new_root) {
                merged_areas++;
                n->neighbours[i]->parent = new_root;
            }
        }
    }
    return merged_areas;
}

static node get_random_neighbour(node n) {
    for (int i = 0; i < 4; i++) {
        if (n->neighbours[i] != NULL && n->neighbours[i]->player->player_index == n->player->player_index) {
            return n->neighbours[i];
        }
    }
    return NULL;

}

static void merge(node new_node) {
    node random_neighbour = get_random_neighbour(new_node);
    if (random_neighbour == NULL) {
        new_node->parent = new_node;
        new_node->player->areas++;
    } else {
        node root = find_root(random_neighbour);
        new_node->parent = root;
        new_node->player->areas -= merge_neighbours(new_node, root);
    }
}

static int number_of_same_color_neighbours(node n) {
    int number = 0;
    for (int i = 0; i < 4; i++) {
        if (n->neighbours[i] != NULL && n->neighbours[i]->player == n->player) {
            number++;
        }
    }
    return number;
}

static void update_edges_when_adding(gamma_t* b, uint32_t player, uint32_t x, uint32_t y) {
    int number_of_new_edges = 0;
    bool at_least_one_neighbour = false;
    node* neighbours = get_neighbours(b, x, y);
    node n;
    for (int i = 0; i < 4; i++) {
        if (x + (i == 0) - (i == 2) < 0 || y + (i == 1) - (i == 3) < 0 || x + (i == 0) - (i == 2) >= b->width ||
            y + (i == 1) - (i == 3) >= b->height) {
            continue;
        }
        n = neighbours[i];
        if (n != NULL) {
            if (n->player->player_index == player) {
                at_least_one_neighbour++;
            } else if (number_of_neighbours_taken_by_player(b, n->player->player_index + 1, x, y) == 1) {
                n->player->area_edges--;
            }
        } else if (!number_of_neighbours_taken_by_player(b, player + 1, x + (i == 0) - (i == 2),
                                                         y + (i == 1) - (i == 3))) {
            number_of_new_edges++;
        }
    }
    b->players[player]->area_edges += number_of_new_edges - at_least_one_neighbour;
}

static void update_edges_when_removing(gamma_t* b, uint32_t player, uint32_t x, uint32_t y) {
    int number_of_new_edges = 0;
    bool at_least_one_neighbour = false;
    node* neighbours = get_neighbours(b, x, y);
    node n;
    for (int i = 0; i < 4; i++) {
        if (x + (i == 0) - (i == 2) < 0 || y + (i == 1) - (i == 3) < 0 || x + (i == 0) - (i == 2) >= b->width ||
            y + (i == 1) - (i == 3) >= b->height) {
            continue;
        }
        n = neighbours[i];
        if (n != NULL) {
            if (n->player->player_index == player) {
                at_least_one_neighbour = true;
            } else if (number_of_neighbours_taken_by_player(b, n->player->player_index + 1, x, y) == 1) {
                n->player->area_edges++;
            }
        } else if (!number_of_neighbours_taken_by_player(b, player + 1, x + (i == 0) - (i == 2),
                                                         y + (i == 1) - (i == 3))) {
            number_of_new_edges++;
        }
    }
    b->players[player]->area_edges -= number_of_new_edges - at_least_one_neighbour;
}
//----------PUBLIC FUNCTIONS------------------------

gamma_t* create_board(uint32_t width, uint32_t height,
                     uint32_t players, uint32_t areas) {
    gamma_t* game = malloc(sizeof(struct gamma));
    game->width = width;
    game->height = height;
    game->number_of_players = players;
    game->max_areas = areas;
    game->players = malloc(players * sizeof(player*));
    for (uint32_t i = 0; i < players; i++) {
        game->players[i] = malloc(sizeof(player));
        game->players[i]->areas = 0;
        game->players[i]->area_edges = 0;
        game->players[i]->player_index = i;
        game->players[i]->pawns_number = 0;
        game->players[i]->golden_move_available = true;
    }
    return game;
}

void delete_board(gamma_t* b) {
    free(b);
}

uint32_t check_field(gamma_t* b, uint32_t x, uint32_t y) {
    node n = get(b->node_tree, x, y);
    if (n == NULL) {
        return 0;
    }
    return n->player->player_index + 1;
}

uint32_t get_number_of_players_areas(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->areas;
}

void add_pawn(gamma_t* b, uint32_t player, uint32_t x, uint32_t y) {
    node newNode = create_node(x, y, b->players[player - 1]);

    node* neighbours = get_neighbours(b, x, y);
    set_neighbours(newNode, neighbours);
    free(neighbours);

    update_edges_when_adding(b, player - 1, x, y);

    merge(newNode);
    add(&(b->node_tree), newNode);
    b->players[player - 1]->pawns_number++;
}

static void remove_pawn_from_neighbours(node n, uint32_t x_to_remove, uint32_t y_to_remove) {
    for (int i = 0; i < 4; i++) {
        if (n->neighbours[i] != NULL && n->neighbours[i]->x == x_to_remove && n->neighbours[i]->y == y_to_remove) {
            n->neighbours[i] = NULL;
            return;
        }
    }
}

void remove_pawn(gamma_t* b, uint32_t x, uint32_t y) {
    node* neighbours = get_neighbours(b, x, y);
    uint32_t player = get(b->node_tree, x, y)->player->player_index;
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL) {
            remove_pawn_from_neighbours(neighbours[i], x, y);
        }
    }
    b->node_tree = delete(b->node_tree, x, y);
    update_edges_when_removing(b, player, x, y);
    b->players[player]->pawns_number--;
}

uint64_t get_number_of_pawns(gamma_t* b) {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < b->number_of_players; i++) {
        sum += b->players[i]->pawns_number;
    }
    return sum;
}

uint64_t get_number_of_players_pawns(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->pawns_number;
}

int number_of_neighbours_taken_by_player(gamma_t* b, uint32_t player, uint32_t x, uint32_t y) {
    node* neighbours = get_neighbours(b, x, y);
    int n = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL && neighbours[i]->player->player_index == player - 1) {
            n++;
        }
    }
    return n;
}

void reset_areas(gamma_t* b) {
    for (uint32_t i = 0; i < b->number_of_players; i++) {
        b->players[i]->areas = 0;
    }
}

void reset_visited(node n) {
    if (!n)
        return;
    n->was_visited = false;
    reset_visited(n->left);
    reset_visited(n->right);
}

void update_areas_dfs(node n, node new_root) {
    if (n->was_visited)
        return;
    n->was_visited = true;
    n->parent = new_root;
    for (int i = 0; i < 4; i++) {
        if (n->neighbours[i] != NULL && n->neighbours[i]->player->player_index == n->player->player_index) {
            update_areas_dfs(n->neighbours[i], new_root);
        }
    }
}

static void traverse_and_update_area(node n) {
    if (n == NULL) {
        return;
    }
    if (n->was_visited == false) {
        n->player->areas++;
        update_areas_dfs(n, n);
    }
    traverse_and_update_area(n->left);
    traverse_and_update_area(n->right);
}

void update_areas(gamma_t* b) {
    reset_areas(b);
    traverse_and_update_area(b->node_tree);
    reset_visited(b->node_tree);
}


uint64_t get_number_of_players_area_edges(gamma_t* b, uint32_t player) {
    return b->players[player - 1]->area_edges;
}

void fill_board(node n, char** buffer) {
    if(!n)
        return;
    buffer[n->y][n->x] = n->player->player_index + 1 + '0';
    fill_board(n->left,buffer);
    fill_board(n->right,buffer);
}

char* print_board(gamma_t* b) {
    char** board = malloc(b->height * sizeof(char*));
    for (uint64_t i = 0; i < b->height; i++) {
        board[i] = malloc((b->width + 1) * sizeof(char));
        for (uint64_t j = 0; j < b->width; j++) {
            board[i][j] = '.';
        }
        board[i][b->width] = '\n';
    }
    fill_board(b->node_tree, board);

    uint64_t length = (b->height * (b->width + 1) + 1) * sizeof(char);
    char* string = malloc(length);
    for (uint64_t i = 0; i < b->height; i++) {
        for (uint64_t j = 0; j < b->width+1; j++) {
            string[i*(b->width+1) + j] = board[b->height - 1 -i][j];
        }
    }
    string[length-1] = '\0';
    return string;
}