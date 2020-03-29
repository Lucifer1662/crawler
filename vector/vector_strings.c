#include "vector_strings.h"
#include "string.h"
#include "stdlib.h"

char* void_ptr_to_str(void* str){
    return *(char**)str;
}

void* indirect_strdup(void* str_ptr){
    return strdup(void_ptr_to_str(str_ptr));
}

void indirect_free(void* str_ptr){
    free(void_ptr_to_str(str_ptr));
}

vector create_vector_strings(){
    vector vec = create_vector(sizeof(char*));
    vec.copy_element = indirect_strdup;
    vec.free_element = indirect_free;
    return vec;
}   

void push_back_str(vector* v, str string){
    push_back(v, &string);
}

void pop_back_str(vector* v, str result);

str at_str(vector* v, int index){
    return *(str*)at(v,index);
}