#include "get_links.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vector/vector_strings.h"

#define ANCHOR_START "<a"
#define ANCHOR_START_CAP "<A"
#define ANCHOR_END ">"
#define HREF "href"
#define QUOTE '"'
#define SINGLE_QUOTE '\''

int is_in_quotes(char *str, char *location, char *end) {
    int res = 0;
    while (1) {
        char *openingQuote = strchr(str, QUOTE);
        if (openingQuote == NULL || openingQuote > end) break;

        // printf("Opening:%s\n",openingQuote);
        // find ending quote
        char *closingQuote = strchr(openingQuote + 1, QUOTE);
        if (closingQuote == NULL || closingQuote > end) break;
        // printf("Closeing:%s\n",closingQuote);

        if (openingQuote < location && location < closingQuote) {
            res = 1;
        }

        str = closingQuote + 1;
    }
    return res;
}

void convert_single_quote_to_quote(char *str) {
    for (; *str; str++) {
        if (*str == SINGLE_QUOTE) {
            *str = QUOTE;
        }
    }
}

void convert_anchor_to_lower(char *str) {
    char *anchor_start_cap;
    int len_anchor_start_cap = strlen(ANCHOR_START_CAP);
    // while there is capital anchors
    while ((anchor_start_cap = strstr(str, ANCHOR_START_CAP)) != NULL) {
        // replace cap with lower
        for (int i = 0; i < len_anchor_start_cap; i++) {
            anchor_start_cap[i] = ANCHOR_START[i];
        }
    }
}

vector get_links(char *html) {
    html = strdup(html);
    convert_single_quote_to_quote(html);
    convert_anchor_to_lower(html);

    vector links = create_vector_strings();
    get_links_symbols(&links, html);

    free(html);
    return links;
}

void get_links_symbols(vector *links, char *html) {
    while (*html) {
        // Find anchor start
        char *anchor_start = strstr(html, ANCHOR_START);
        if (anchor_start == NULL) {
            break;
        }

        // Find anchor end
        char *anchor_end = strstr(anchor_start, ANCHOR_END);
        if (anchor_end == NULL) break;

        // find href and check it is inside anchor

        char *href = anchor_start;

        int valid_href = 0;
        while (1) {
            href = strcasecmp(href, HREF);

            if (href == NULL) break;
            if (anchor_start < href && href < anchor_end &&
                !is_in_quotes(anchor_start, href, anchor_end)) {
                valid_href = 1;
                break;
            }
            href += strlen(HREF);
        }

        if (valid_href) {
            // if inside
            // find opening quote
            char *openingQuote = strchr(href, QUOTE) + 1;
            if (openingQuote == NULL) break;

            // find ending quote
            char *closingQuote = strchr(openingQuote, QUOTE);
            if (closingQuote == NULL) break;

            // extract link and push
            int linkLength = closingQuote - openingQuote;
            char *link = strndup(openingQuote, linkLength);
            push_back_str(links, link);
        }
        // move html to end of anchor
        html = anchor_end + 1;
    }
}