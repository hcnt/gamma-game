#include "board.h"


static node* get_neighbours(board_t b, uint32_t x, uint32_t y) {
    node* neighbours = calloc(4, sizeof(node));
    if (x < b->height) {
        neighbours[0] = find(b->node_tree, x + 1, y);
    }
    if (y < b->width) {
        neighbours[1] = find(b->node_tree, x, y + 1);
    }
    if (x > 0) {
        neighbours[2] = find(b->node_tree, x - 1, y);
    }
    if (y > 0) {
        neighbours[3] = find(b->node_tree, x, y - 1);
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


//----------PUBLIC FUNCTIONS------------------------

board_t create_board(uint32_t width, uint32_t height,
                     uint32_t players, uint32_t areas) {
    board_t game = malloc(sizeof(struct board));
    game->width = width;
    game->height = height;
    game->players_number = players;
    game->max_areas = areas;
    game->players = malloc(players * sizeof(player*));
    for (uint32_t i = 0; i < players; i++) {
        game->players[i] = malloc(sizeof(player));
        game->players[i]->areas = 0;
        game->players[i]->player_index = i;
        game->players[i]->pawns_number = 0;
        game->players[i]->golden_move_available = true;
    }
    return game;
}

void delete_board(board_t b) {
    free(b);
}

uint32_t check_field(board_t b, uint32_t x, uint32_t y) {
    node n = find(b->node_tree, x, y);
    if (n == NULL) {
        return 0;
    }
    return n->player->player_index + 1;
}

uint32_t get_number_of_players_areas(board_t b, uint32_t player) {
    return b->players[player - 1]->areas;
}

void add_pawn(board_t b, uint32_t player, uint32_t x, uint32_t y) {
    node newNode = create_node(x, y, b->players[player - 1]);

    node* neighbours = get_neighbours(b, x, y);
    for (int i = 0; i < 4; ++i) {
        if (neighbours[i] != NULL) {
            newNode->neighbours[i] = neighbours[i];
            neighbours[i]->neighbours[(i + 2) % 4] = newNode;
        }
    }
    free(neighbours);

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

void remove_pawn(board_t b, uint32_t x, uint32_t y) {
    node* neighbours = get_neighbours(b, x, y);
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL) {
            remove_pawn_from_neighbours(neighbours[i], x, y);
        }
    }
    b->node_tree = delete(b->node_tree, x, y);
}

uint64_t get_number_of_pawns(board_t b) {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < b->players_number; i++) {
        sum += b->players[i]->pawns_number;
    }
    return sum;
}

uint64_t get_number_of_players_pawns(board_t b, uint32_t player) {
    return b->players[player - 1]->pawns_number;
}

bool check_if_any_neighbour_is_taken_by_player(board_t b, uint32_t player, uint32_t x, uint32_t y) {
    node* neighbours = get_neighbours(b, x, y);
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL && neighbours[i]->player->player_index == player - 1) {
            return true;
        }
    }
    return false;
}

void reset_areas(board_t b) {
    for (uint32_t i = 0; i < b->players_number; i++) {
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

void update_areas(board_t b) {
    reset_areas(b);
    traverse_and_update_area(b->node_tree);
    reset_visited(b->node_tree);
}
