#include "get_links.h"
#include <pcre.h>   /* PCRE lib        NONE  */
#include <stdio.h>  /* I/O lib         C89   */
#include <stdlib.h> /* Standard Lib    C89   */
#include <string.h>
#include "../vector/vector_strings.h"

//returns a vector of char*
vector get_links(char *html)
{
    vector links = create_vector_strings();
    while (1){
        char *href = strstr(html, "href");
        if (href == NULL)
        {
            printf("No HREF");
            break;
        }

        char *openingQuote = strchr(href, '"') + 1;
        if (openingQuote == NULL)
            break;

        char *closingQuote = strchr(openingQuote, '"');
        if (closingQuote == NULL)
            break;

        int linkLength = closingQuote - openingQuote;
        
        char *link = strndup(openingQuote, linkLength);
        push_back(&links, &link);

        html = closingQuote + 1;
    }
    return links;
}