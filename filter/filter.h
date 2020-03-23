#ifndef FILTER
#define FILTER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../vector/vector.h"
#include "../vector/vector_strings.h"

typedef int (*Boolean_Predicate)(void*);



void filter(vector* data, Boolean_Predicate predicate){
    for(int i = 0; i < data->size;){
        if(!predicate(at(data, i))){
            printf("    Removing: %s\n", at_str(data, i));
            swap_pop(data, i);
        }else{
            i++;
        }
    }
}

#endif