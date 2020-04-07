#ifndef GET_LINKS
#define GET_LINKS
#include "../vector/vector.h"


void get_links_symbols(vector* links, char *html);
//extracts the links from the html
vector get_links(char* html);

#endif