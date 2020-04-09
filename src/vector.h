#ifndef ZAD1_VECTOR_H
#define ZAD1_VECTOR_H

#include <stdlib.h>

#define START_CAPACITY 128
#define generate_vector_header(TYPE)                                             \
    struct vector##TYPE {                                                      \
        TYPE* array;                                                           \
        int n;                                                                 \
        int capacity;                                                          \
    };                                                                         \
    vector##TYPE init_vector##TYPE();                                           \
void append_vector##TYPE(vector##TYPE* v, TYPE a);                              \
void pop_vector##TYPE(vector##TYPE* v);                                         \
TYPE get_vector##TYPE(vector##TYPE v, int i);                                   \
void free_vector##TYPE(vector##TYPE* v);


#define generate_vector_type(TYPE)                                               \
    vector##TYPE init_vector##TYPE() {                                          \
        vector##TYPE v;                                                        \
        v.array = safeMalloc(START_CAPACITY * sizeof(TYPE));                   \
        v.capacity = START_CAPACITY;                                           \
        v.n = 0;                                                               \
        return v;                                                              \
    }                                                                          \
    void append_vector##TYPE(vector##TYPE* v, TYPE a) {                         \
        if (v->capacity == v->n) {                                             \
            v->array = safeRealloc(v->array, 2 * v->capacity * sizeof(TYPE));  \
            v->capacity = 2 * v->capacity;                                     \
        }                                                                      \
        v->array[v->n] = a;                                                    \
        (v->n)++;                                                              \
    }                                                                          \
    void pop_vector##TYPE(vector##TYPE* v) {                                    \
        if (v->n != 0) {                                                       \
            (v->n)--;                                                          \
        }                                                                      \
    }                                                                          \
    TYPE get_vector##TYPE(vector##TYPE v, int i) {                              \
        if (i >= v.n) {                                                        \
            printf("ERROR, INDEX OUT OF BOUNDS\n");                            \
        }                                                                      \
        return v.array[i];                                                     \
    }                                                                          \
    void free_vector##TYPE(vector##TYPE* v) { free(v->array);v->array = NULL;v->n=0; }


typedef struct vectorchar vectorchar;

void* safeMalloc(size_t size);

void* safeRealloc(void* oldPtr, size_t size);

generate_vector_header(char)

#endif //ZAD1_VECTOR_H

