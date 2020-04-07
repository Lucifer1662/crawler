#include "get_links.h"
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
char *strcasestr(const char *haystack, const char *needle);
#include <string.h>
#include "../vector/vector_strings.h"

#define ANCHOR_START "<a"
#define ANCHOR_START_CAP "<A"
#define ANCHOR_END ">"
#define HREF "href"
#define HREF_CAP "HREF"
#define QUOTE '"'
#define SINGLE_QUOTE '\''
#define TRUE 1
#define FALSE 0
typedef int bool;

bool is_in_quotes(char *str, char *location, char *end) {
    bool res = FALSE;
    while (1) {
        char *openingQuote = strchr(str, QUOTE);
        if (openingQuote == NULL || openingQuote > end) break;

        // find ending quote
        char *closingQuote = strchr(openingQuote + 1, QUOTE);
        if (closingQuote == NULL || closingQuote > end) break;

        if (openingQuote < location && location < closingQuote) {
            res = TRUE;
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

vector get_links(char *html) {
    html = strdup(html);
    convert_single_quote_to_quote(html);

    vector links = create_vector_strings();
    get_links_symbols(&links, html);

    free(html);
    return links;
}

void get_links_symbols(vector *links, char *html) {
    while (*html) {
        // Find anchor start
        char *anchor_start = strcasestr(html, ANCHOR_START);
        if (anchor_start == NULL) {
            break;
        }

        // Find anchor end
        char *anchor_end = strstr(anchor_start, ANCHOR_END);
        if (anchor_end == NULL) break;

        // find href and check it is inside anchor

        char *href = anchor_start;

        bool valid_href = FALSE;
        while (1) {
            href = strcasestr(href, HREF);

            if (href == NULL) break;
            if (anchor_start < href && href < anchor_end &&
                !is_in_quotes(anchor_start, href, anchor_end)) {
                valid_href = TRUE;
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