/**@file
 * implements macro to create dynamic array for any type
 */
#ifndef ZAD1_VECTOR_H
#define ZAD1_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>

/**
 * vector starts with this capacity
 */
#define START_CAPACITY 128


/**
 * generates vector header for given type
 */
#define generate_vector_header(TYPE)                                           \
    struct vector##TYPE {                                                      \
        TYPE* array;                                                           \
        int n;                                                                 \
        int capacity;                                                          \
    };                                                                         \
    vector##TYPE init_vector##TYPE();                                          \
bool append_vector##TYPE(vector##TYPE* v, TYPE a);                             \
void pop_vector##TYPE(vector##TYPE* v);                                        \
TYPE get_vector##TYPE(vector##TYPE v, int i);                                  \
void free_vector##TYPE(vector##TYPE* v);


/**
 * generates definitions for functions for given vector type
 */
#define generate_vector_type(TYPE)                                             \
    vector##TYPE init_vector##TYPE() {                                         \
        vector##TYPE v;                                                        \
        v.array = malloc(START_CAPACITY * sizeof(TYPE));                       \
        v.capacity = START_CAPACITY;                                           \
        v.n = 0;                                                               \
        return v;                                                              \
    }                                                                          \
    bool append_vector##TYPE(vector##TYPE* v, TYPE a) {                        \
        if (v->capacity == v->n) {                                             \
            v->array = realloc(v->array, 2 * v->capacity * sizeof(TYPE));      \
            v->capacity = 2 * v->capacity;                                     \
        }                                                                      \
        if(v->array == NULL){                                                  \
            return false;                                                      \
        }                                                                      \
        v->array[v->n] = a;                                                    \
        (v->n)++;                                                              \
        return true;                                                           \
    }                                                                          \
    void pop_vector##TYPE(vector##TYPE* v) {                                   \
        if (v->n != 0) {                                                       \
            (v->n)--;                                                          \
        }                                                                      \
    }                                                                          \
    TYPE get_vector##TYPE(vector##TYPE v, int i) {                             \
        if (i >= v.n) {                                                        \
            printf("ERROR, INDEX OUT OF BOUNDS\n");                            \
        }                                                                      \
        return v.array[i];                                                     \
    }                                                                          \
    void free_vector##TYPE(vector##TYPE* v) { free(v->array);v->array = NULL;v->n=0; }

/**
 * create char vector type
 */
typedef struct vectorchar vectorchar;

generate_vector_header(char)

#endif //ZAD1_VECTOR_H

