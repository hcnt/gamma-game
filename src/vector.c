#include "vector.h"
#include "stdio.h"
#include "stdlib.h"

void* safeMalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        exit(1);
    }
    return ptr;
}

void* safeRealloc(void* oldPtr, size_t size) {
    void* ptr = realloc(oldPtr, size);
    if (ptr == NULL) {
        exit(1);
    }
    return ptr;
}

generate_vector_type(char)
