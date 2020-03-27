#ifndef NODE_H
#define NODE_H

#include "player.h"
typedef struct pawn* node;
struct pawn {
    uint32_t x;
    uint32_t y;
    node parent;
    node left;
    node right;
    node neighbours[4];
    player* player;
    bool was_visited;
};

node create_node(uint32_t x, uint32_t y,player* p);

void add(node* root_ptr, node element);

node find(node root, uint32_t x, uint32_t y);

bool is_bigger(node a, uint32_t x, uint32_t y);

node delete(node t, uint32_t x, uint32_t y);

#endif