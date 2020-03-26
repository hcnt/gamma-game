#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"

node create_node(uint32_t x, uint32_t y) {
    node newNode = malloc(sizeof(struct pawn));
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = newNode;
    newNode->x = x;
    newNode->y = y;
    return newNode;
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
    } else {
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

void free_node(node n) {
    if (!n)
        return;
    free_node(n->left);
    free_node(n->right);
    free(n);
}

static node min_node(node t) {
    while (t != NULL && t->left != NULL) {
        t = t->left;
    }
    return t;
}

node delete(node t, uint32_t x, uint32_t y) {
    if (t == NULL)
        return NULL;
    if (t->x == x && t->y == y) {
        if (t->left == NULL && t->right == NULL) {
            free(t);
            return NULL;
        } else if (t->left == NULL) {
            node tmp = t->right;
            free(t);
            t = tmp;
        } else if (t->right == NULL) {
            node tmp = t->left;
            free(t);
            t = tmp;
        } else {
            node tmp = min_node(t->right);
            t->x = tmp->x;
            t->y = tmp->y;
            t->parent = tmp->parent;
            t->right = delete(t->right, tmp->x, tmp->y);
        }

    } else if (is_bigger(t, x, y)) {
        t->left = delete(t->left, x, y);
    } else {
        t->right = delete(t->right, x, y);
    }
    return t;
}
