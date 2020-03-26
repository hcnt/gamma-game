#include "gamma.h"
#include "stdlib.h"
#include "node.h"

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;
    node* players_pawns;
    uint32_t* players_areas;
};

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
    game->players = players;
    game->players_pawns = malloc(players * sizeof(node));
    game->players_areas = calloc(players, sizeof(uint32_t));
    game->areas = areas;
    return game;
}

void gamma_delete(gamma_t* g) {
    if (g != NULL) {
        free(g);
    }
}

static bool is_node_in_any_player(gamma_t* g, uint32_t x, uint32_t y) {
    for (uint32_t i = 0; i < g->players; i++) {
        if (find(g->players_pawns[i], x, y) != NULL) {
            return true;
        }
    }
    return false;
}

static node* get_neighbours(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    node* neighbours = calloc(4,sizeof(node));
    if (x < g->height) {
        neighbours[0] = find(g->players_pawns[player],x+1,y);
    }
    if (y < g->width) {
        neighbours[1] = find(g->players_pawns[player],x,y+1);
    }
    if (x > 0) {
        neighbours[2] = find(g->players_pawns[player],x-1,y);
    }
    if (y > 0) {
        neighbours[3] = find(g->players_pawns[player],x,y-1);
    }
    return neighbours;
}

static bool is_added_to_area(node* neighbours){
    for(int i = 0; i<4;i++){
        if(neighbours[i] != NULL){
            return true;
        }
    }
    return false;
}

static node get_random_neighbour(node* neighbours){
    for(int i = 0; i<4;i++){
        if(neighbours[i] != NULL){
            return neighbours[i];
        }
    }
    return NULL;

}

static bool are_gamma_move_parameters_valid(gamma_t* g, uint32_t player, uint32_t x, uint32_t y){
    if(g == NULL){
        return false;
    }
    if(player>g->players){
        return false;
    }
    if(x>g->width){
        return false;
    }
    if(y>g->height){
        return false;
    }
    return true;
}

//TODO add merging areas together
bool gamma_move(gamma_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!are_gamma_move_parameters_valid(g, player, x, y)) {
        return false;
    }
    if (is_node_in_any_player(g, x, y)) {
        return false;
    }
    node* neighbours = get_neighbours(g,player, x, y);
    bool added_to_area_flag = is_added_to_area(neighbours);
    if (g->players_areas[player] == g->areas && !added_to_area_flag) {
        return false;
    }
    node newNode = create_node(x, y);
    if (!added_to_area_flag) {
        (g->players_areas[player])++;
    } else {
        newNode->parent = get_random_neighbour(neighbours);
    }
    add(&(g->players_pawns[player]), newNode);
    return true;
}
