#ifndef STR_UTIL_H
#define STR_UTIL_H

//param 1, number of strings,
//then the strigns to concat
//returns a malloced new string as the result
char* concat_create(int count, ...);

//makes characters in the string to 
//lower case
void str_to_lower(char* str);

#endif