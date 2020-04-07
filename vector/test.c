#include "vector.h"
#include "stdio.h"
#include "assert.h"

void main(){
    printf("Test Start\n");
    vector v = create_vector(sizeof(int));
    printf("Created Vector\n");
    int item = 5;
    push_back(&v, &item);
    push_back(&v, &item);
    item = 3;
    push_back(&v, &item);
    printf("Add items\n");
    int i = 0;
    assert(v.size == 3);
    assert(*(int*)at(&v, i++) == 5);
    assert(*(int*)at(&v, i++) == 5);
    assert(*(int*)at(&v, i++) == 3);
    assert(v.size == 3);
    printf("Popping Back");
    int res;
    pop_back(&v, &res);
    assert(v.size == 2);
    assert(res == 3);
    free_vector(&v);
}