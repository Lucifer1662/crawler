#include "vector.h"
#include "stdlib.h"
#include "string.h"
#include "vector_strings.h"

void *no_copy(void *src)
{
    return src;
}

vector create_vector(int sizeOfItem)
{
    vector vec;
    vec.capacity = 1;
    vec.size = 0;
    vec.sizeOfItem = sizeOfItem;
    vec.data = malloc(vec.capacity * sizeOfItem);
    vec.copy_element = no_copy;
    vec.free_element = NULL;
    return vec;
}

void increase_cap(vector *v)
{
    v->capacity *= 2;
    v->data = realloc(v->data, v->capacity * v->sizeOfItem);
}

void *at(vector *v, int index)
{
    return (void *)((char *)v->data + (index * v->sizeOfItem));
}

void push_back(vector *v, void *item)
{
    if (v->size == v->capacity)
    {
        increase_cap(v);
    }

    void *end = at(v, v->size);
    memcpy(end, item, v->sizeOfItem);
    v->size++;
}

void pop_back_item(vector *v, void *item)
{
    v->size--;
    void *end = at(v, v->size);
    memcpy(item, end, v->sizeOfItem);
}

void pop_back(vector *v)
{
    v->size--;
    void *end = at(v, v->size);
    if (v->free_element)
    {
        v->free_element(end);
    }
}

void free_vector(vector *v)
{
    if (v->free_element != NULL)
    {
        for (int i = 0; i < v->size; i++)
        {
            v->free_element(at(v, i));
        }
    }

    free(v->data);
}

void same_operators(vector *src, vector *dst)
{
    dst->copy_element = src->copy_element;
    dst->free_element = src->free_element;
}

void swap_pop(vector *v, int i)
{
    if (v->free_element)
    {
        v->free_element(at(v, i));
    }
    memcpy(at(v, i), at(v, v->size - 1), v->sizeOfItem);
    v->size--;
}

int vector_any(vector *v, void *value, int (*predicate)(void *, void *))
{
    for (int i = 0; i < v->size; i++)
    {
        if (predicate(value, at(v, i)))
        {
            return 1;
        }
    }
    return 0;
}

int vector_all(vector *v, void *value, int (*predicate)(void *, void *)){
    for (int i = 0; i < v->size; i++)
    {
        if (!predicate(value, at(v, i)))
        {
            return 0;
        }
    }
    return 1;
}



