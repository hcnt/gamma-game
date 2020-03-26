#include "gamma.h"
#include "stdlib.h"
#include "node.h"
#include "player.h"
#include "board.h"

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players_number;
    uint32_t areas;
    player* players;
    board_t b;
};

static player get_player(gamma_t* g, uint32_t i) {
    return g->players[i];
}

static bool areParametersForNewGameValid(uint32_t width, uint32_t height,
                                         uint32_t players) {
    if (width <= 0) {
        return false;
    }
    if (height <= 0) {
        return false;
    }
    if (players <= 0) {
        return false;
    }
    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (!areParametersForNewGameValid(width, height, players)) {
        return NULL;
    }
    gamma_t* game = malloc(sizeof(gamma_t));
    game->width = width;
    game->height = height;
    game->players_number = players;
    game->areas = areas;
    game->players = malloc(players * sizeof(player));
    for (uint32_t i = 0; i < players; i++) {
        game->players[i].areas = 0;
        game->players[i].pawns_number = 0;
        game->players[i].pawns = NULL;
        game->players[i].golden_move_available = true;
    }
    return game;
}

void gamma_delete(gamma_t* g) {
    if (g != NULL) {
        free(g);
    }
}

static bool is_node_in_any_player(gamma_t* g, uint32_t x, uint32_t y) {
    for (uint32_t i = 0; i < g->players_number; i++) {
        if (find(get_player(g, i).pawns, x, y) != NULL) {
            return true;
        }
    }
    return false;
}

static node* get_neighbours(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    node* neighbours = calloc(4, sizeof(node));
    if (x < g->height) {
        neighbours[0] = find(get_player(g, player).pawns, x + 1, y);
    }
    if (y < g->width) {
        neighbours[1] = find(get_player(g, player).pawns, x, y + 1);
    }
    if (x > 0) {
        neighbours[2] = find(get_player(g, player).pawns, x - 1, y);
    }
    if (y > 0) {
        neighbours[3] = find(get_player(g, player).pawns, x, y - 1);
    }
    return neighbours;
}

static bool is_added_to_area(node* neighbours) {
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL) {
            return true;
        }
    }
    return false;
}

static node get_random_neighbour(node* neighbours) {
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL) {
            return neighbours[i];
        }
    }
    return NULL;

}

static node find_root(node n) {
    node tmp = n;
    while (tmp->parent != tmp) {
        tmp = tmp->parent;
    }
    n->parent = tmp;
    return tmp;
}

//static int merge_neighbours(node new_root, node* neighbours) {
//    node tmp;
//    int merged_areas = 0;
//    for (int i = 0; i < 4; i++) {
//        if (neighbours[i] != NULL) {
//            tmp = find_root(neighbours[i]);
//            if (tmp != new_root) {
//                merged_areas++;
//                neighbours[i]->parent = new_root;
//            }
//        }
//    }
//    return merged_areas;
//}
static int merge_neighbours(node new_root, node root, node* neighbours) {
    node tmp;
    int merged_areas = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL) {
            tmp = find_root(neighbours[i]);
            if (tmp != root) {
                merged_areas++;
                neighbours[i]->parent = new_root;
            }
        }
    }
    return merged_areas;
}

//returns number of merged areas
static int merge(node new_node, node* neighbours) {
    node random_neighbour = get_random_neighbour(neighbours);
    new_node->parent = random_neighbour;
    node root = find_root(random_neighbour);
    return merge_neighbours(new_node, root, neighbours);
}

static bool are_gamma_move_parameters_valid(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL) {
        return false;
    }
    if (player > g->players_number) {
        return false;
    }
    if (player == 0) {
        return false;
    }
    if (x > g->width) {
        return false;
    }
    if (y > g->height) {
        return false;
    }
    return true;
}

bool gamma_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!are_gamma_move_parameters_valid(g, player, x, y)) {
        return false;
    }
    player -= 1;
    if (check_field(g->b,x,y) == 0 ) {
        return false;
    }
    bool added_to_area_flag = check_if_any_neighbour_is_taken_by_player(g->b, player, x, y);
    if(get_number_of_players_areas(g->b,player) == g->areas && added_to_area_flag){
        return false;
    }
    add_pawn(g->b,x,y);
    return true;
}

//TODO implement removing node
static uint32_t remove_node(gamma_t* g, uint32_t player, uint32_t x, uint32_t y);

bool gamma_golden_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL) return false;
    if (player > g->players_number || player == 0) {
        return false;
    }
    player -= 1;

    if (!g->players[player].golden_move_available) {
        return false;
    }

    node node_to_remove = NULL;
    uint32_t i = 0;
    while (i < g->players_number && node_to_remove == NULL) {
        node_to_remove = find(g->players[i].pawns, x, y);
        i++;
    }
    //i is one too big after last loop
    i--;

    if (node_to_remove == NULL) return false;
    if (i == player) return false;

//    remove_node(g,i,x,y);
    return true;
}

uint64_t gamma_busy_fields(gamma_t* g, uint32_t player) {
    if (g == NULL) return 0;
    if (player > g->players_number || player == 0) {
        return 0;
    }
    player -= 1;
    return g->players[player].pawns_number;
}

bool gamma_golden_possible(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return false;
    if (player > g->players_number || player == 0) {
        return false;
    }
    player -= 1;
    if (!g->players[player].golden_move_available) return false;
    for (uint32_t i = 0; i < g->players_number; i++) {
        if (i != player && get_player(g, i).pawns_number > 0) {
            return true;
        }
    }
    return false;
}

uint64_t gamma_free_fields(gamma_t* g, uint32_t player) {
    if (g == NULL)
        return 0;
    if (player > g->players_number || player == 0) {
        return 0;
    }
    player -= 1;
    if (g->players[player].areas == g->areas) {
        //TODO implement counting areas borders
        return 0;
    }
    uint64_t sum = 0;
    for (uint32_t i = 0; i < g->players_number; i++) {
        sum += g->players[i].pawns_number;
    }
    return g->height * g->width - sum;
}
