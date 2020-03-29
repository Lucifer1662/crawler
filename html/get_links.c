#include "get_links.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vector/vector_strings.h"

#define ANCHOR_START "<a"
#define ANCHOR_END ">"
#define HREF "href"
#define QUOTE '"'

// returns a vector of char*
vector get_links(char *html) {
    vector links = create_vector_strings();
    while (*html) {
        //Find anchor start
        char *anchor_start = strstr(html, ANCHOR_START);
        if (anchor_start == NULL) break;

        //Find anchor end
        char *anchor_end = strstr(anchor_start, ANCHOR_END);
        if(anchor_end == NULL) break;

        //find href and check it is inside anchor
        char *href = strstr(anchor_start, HREF);
        if (href == NULL) break;
        if (anchor_start < href && href < anchor_end) {
            //if inside
            //find opening quote
            char *openingQuote = strchr(href, QUOTE) + 1;
            if (openingQuote == NULL) break;

            //find ending quote
            char *closingQuote = strchr(openingQuote, QUOTE);
            if (closingQuote == NULL) break;

            //extract link and push
            int linkLength = closingQuote - openingQuote;
            char *link = strndup(openingQuote, linkLength);
            push_back(&links, &link);
        }
        //move html to end of anchor
        html = anchor_end + 1;
    }
    return links;
}