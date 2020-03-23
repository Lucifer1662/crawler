#ifndef FETCH_PAGE
#define FETCH_PAGE
#include "../uri/uri.h"

struct Http_Response_Header_t{
    int statusCode;
};
typedef struct Http_Response_Header_t Http_Response_Header;

struct Http_Response_T
{
    Http_Response_Header header;
    char* body;
};
typedef struct Http_Response_T Http_Response;



Http_Response fetch_page_url(char* url);
Http_Response fetch_page_uri(Uri uri);

#endif