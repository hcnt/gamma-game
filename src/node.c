#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"

node create_node(uint32_t x,uint32_t y){
    node newNode = malloc(sizeof(struct pawn));
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->x = x;
    newNode->y = y;
}

bool is_bigger(node a, uint32_t x, uint32_t y) {
    if (a == NULL) {
        fprintf(stderr, "Comparison between null pointers");
        exit(1);
    }
    if (a->x > x) {
        return true;
    } else if (a->x < x) {
        return false;
    }
    if (a->y > y) {
        return true;
    } else if (a->y <= y) {
        return false;
    }
}

node find(node root, uint32_t x, uint32_t y) {
    if (root == NULL) {
        return NULL;
    }
    if (root->x == x && root->y == y) {
        return root;
    }
    if (is_bigger(root, x, y)) {
        return find(root->left, x, y);
    }
    return find(root->right, x, y);
}

void add(node* root_ptr, node element) {
    if (root_ptr == NULL) {
        fprintf(stderr, "NULL POINTER EXCEPTION");
        exit(1);
    }
    if (*root_ptr == NULL) {
        *root_ptr = element;
        return;
    }
    if (is_bigger(element, (*root_ptr)->x, (*root_ptr)->y)) {
        add(&(*root_ptr)->right, element);
        return;
    }
    add(&(*root_ptr)->left, element);
}
