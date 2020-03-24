#include "gamma.h"
#include "stdlib.h"

static bool areParametersForNewGameValid(uint32_t width, uint32_t height,
                                         uint32_t players, uint32_t areas){
    if(width <= 0){
        return false;
    }
    if(height <= 0){
        return false;
    }
    if(players <= 0){
        return false;
    }
    if(areas < 0){
        return false;
    }
    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas){
    if(!areParametersForNewGameValid(width,height,players,areas)){
        return NULL;
    }
    gamma_t* game = malloc(sizeof(gamma_t));
    game->width = width;
    game->height = height;
    game->players = players;
    game->areas = areas;
    return game;
}

void gamma_delete(gamma_t* g){
    if(g != NULL){
        free(g);
    }
}
