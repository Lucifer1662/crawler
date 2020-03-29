#ifndef VECTOR
#define VECTOR

struct vector_t {
    void* data;
    int size;
    int capacity;
    int sizeOfItem;
    void* (*copy_element)(void* src);
    void (*free_element)(void*);
};

typedef struct vector_t vector;

vector create_vector(int sizeOfItem);

void increase_cap(vector* v);

void push_back(vector* v, void* item);

void pop_back_free(vector* v);

void pop_back(vector* v, void* back);

void free_vector(vector* v);

void* at(vector* v, int index);

void same_operators(vector* src, vector* dst);

void swap_pop(vector* v, int i);

int vector_any(vector* v, void* value, int (*predicate)(void*, void*));

int vector_all(vector* v, void* value, int (*predicate)(void*, void*));

#define map_inplace(vec_ptr, transform, ...)    \
    for (int i = 0; i < (vec_ptr)->size; i++) { \
        transform(at(vec_ptr, i), __VA_ARGS__); \
    }

#define any_return(vec_ptr, at_func, predicate, ...)       \
    for (int i = 0; i < (vec_ptr)->size; i++) {            \
        if (predicate(at_func(vec_ptr, i), __VA_ARGS__)) { \
            return 1;                                      \
        }                                                  \
    }

#define filter_inplace_n(vec_ptr, at_func, predicate, ...)  \
    for (int i = 0; i < (vec_ptr)->size;) {                 \
        if (!predicate(at_func(vec_ptr, i), __VA_ARGS__)) { \
            swap_pop(vec_ptr, i);                           \
        } else {                                            \
            i++;                                            \
        }                                                   \
    }

#define filter_inplace(vec_ptr, at_func, predicate) \
    for (int i = 0; i < (vec_ptr)->size;) {         \
        if (!predicate(at_func(vec_ptr, i))) {      \
            swap_pop(vec_ptr, i);                   \
        } else {                                    \
            i++;                                    \
        }                                           \
    }

#endif