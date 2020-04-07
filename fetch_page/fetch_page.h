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

/*
Fetches a page with url and auth
 - if no auth, then auth = NULL
 - the body is only present if the mimetype is text/html
 else the body is NULL
*/
Http_Response fetch_page_url(char* url, Authorization* auth);

/*
Fetches a page with url and auth
 - if no auth, then auth = NULL
 - the body is only present if the mimetype is text/html
 else the body is NULL
*/
Http_Response fetch_page_uri(Uri uri, Authorization* auth);

#endif