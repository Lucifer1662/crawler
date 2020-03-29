#ifndef FETCH_PAGE
#define FETCH_PAGE
#include "../uri/uri.h"
#include "http_response.h"


struct Authorization_t
{
    char* userid_password_64;
};
typedef struct Authorization_t Authorization;

Authorization create_auth(char* userid_password_64);


void free_http_response_header(Http_Response_Header header);
void free_http_response(Http_Response response);

Http_Response fetch_page_url(char* url, Authorization* auth);
Http_Response fetch_page_uri(Uri uri, Authorization* auth);

#endif