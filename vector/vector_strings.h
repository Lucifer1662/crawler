#ifndef VECTOR_STRINGS
#define VECTOR_STRINGS
#include "vector.h"
typedef char* str;

vector create_vector_strings();

void push_back_str(vector* v, str item);

void pop_back_str(vector* v, str result);

str at_str(vector* v, int index);

#endif