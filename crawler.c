#include "crawler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "fetch_page/fetch_page.h"
#include "html/get_links.h"
#include "uri/uri.h"
#include "vector/vector.h"
#include "vector/vector_strings.h"

#define OK_STATUS 200
#define NOT_FOUND_STATUS 404
#define GONE_STATUS 410
#define URI_TOO_LONG_STATUS 414
#define SERVICE_UNAVAILABLE_STATUS 503
#define GATEWAY_TIMEOUT_STATUS 504
#define MOVED_PERMENTLY_STATUS 301
#define UNATHORIZED_STATUS 401

#define REDIRECT_CODE 2
#define TRY_AGAIN_AGAIN 1
#define FINISHED_CODE 0
#define USERNAME "lmhawkins"
#define PASSWORD "password"
#define AUTH_64_ENCODED "bG1oYXdraW5zOnBhc3N3b3Jk"

int str_equal(str str1, str str2) { return strcmp(str1, str2) == 0; }

int is_str_in(vector *vec, str str) {
    any_return(vec, at_str, str_equal, str);
    return 0;
}

/*
Takes a response and adds the urls inside the page
to the queue
*/
void add_links_from_html(Http_Response response, char *url,
                         vector *queueSites) {
    char *host = get_host(url);
    vector links = get_links(response.body);

    //refactor urls to be absolute
    map_inplace(&links, make_absolute_url, host, url);

    // filter out invalid responses, such as containing
    //fragments or encoded characters, or relative urls.
    filter_inplace(&links, at_str, is_valid_url);

    //Add urls that arent already in queue
    for (int i = 0; i < links.size; i++) {
        if (!is_str_in(queueSites, at_str(&links, i))) {
            char *url = queueSites->copy_element(at(&links, i));
            push_back(queueSites, &url);
        }
    }
    free_vector(&links);
    free(host);
}

/*
Why am I handling the error codes like this?

OK_STATUS 200:
If everything is okay then it is fine to traverse the page
and extract the urls.


NOT_FOUND_STATUS 404
If the url is not found then it is still fine to traverse the error
page and add the urls
and then the program should continue, as
there is nothing i can do to resolve this error

GONE_STATUS 410
If the page use to exists here but is gone for the
unseeable future.
There is no way for me to know if it is temporary
or permanent and no way to resolve that.
Thus the program should just continue on as it has no
alternative

URI_TOO_LONG_STATUS 414
If the uri I sent to the page is too long
this one i could handle if i could figure out what the uri
is actually meant to be, as this error usually results from
a client-side bug. However i could not figure out a determinsitic way
to correct (if it could even be corrected). Thus
the program just traverses the page and continues.

SERVICE_UNAVAILABLE_STATUS 503
This error should be used when the service is temporary
unavailable, hence all the client has to do is wait.
But i am too impatient for that, and i can just request
the page straight away, as per a discussion on the
discussion board.

GATEWAY_TIMEOUT_STATUS 504
Indicates that the server, while acting as a gateway or proxy,
did not get a response in time from the upstream server that
it needed in order to complete the request. Thus this
is a server side error and not much can be done and
the problem could be only temporary. Thus it is fine to
try again. Why will this not get stuck in an infinite loop?
The spec says it won't.


MOVED_PERMENTLY_STATUS 301
Indicates that the page has been moved permanently to another page
and this new page is located in the "location" line in the header
Thus the program traverses this page and pushes this location to the
queue and will process it later, as it does not have to do it
straight away.


UNATHORIZED_STATUS 401
This code lets us know that we cannot access this page
without some form of credentials. Thus we traverse this page
grab its links, and immediately try again with authorization
credentials enabled.
*/

/*
Adds links in the response to the queueSites
Simplifies the response code into 3 states
and returns 1 of REDIRECT_CODE TRY_AGAIN_AGAIN FINISHED_CODE
*/
int handle_response(Http_Response response, char *url, vector *queueSites,
                    Authorization *auth) {
    if (response.body != NULL) add_links_from_html(response, url, queueSites);

    switch (response.header.statusCode) {
        case OK_STATUS:
        case NOT_FOUND_STATUS:
        case URI_TOO_LONG_STATUS:
        case GONE_STATUS:
            return FINISHED_CODE;

        case UNATHORIZED_STATUS:
            if (auth->userid_password_64 == NULL) {
                *auth = create_auth(AUTH_64_ENCODED);
            } else {
                // we have already tried and our credentials are wrong
                return FINISHED_CODE;
            }
        case SERVICE_UNAVAILABLE_STATUS:
        case GATEWAY_TIMEOUT_STATUS:
            return TRY_AGAIN_AGAIN;

        case MOVED_PERMENTLY_STATUS: {
            char *redirect = response.header.redirect;
            if (!is_str_in(queueSites, redirect)) {
                push_back_str(queueSites, strdup(redirect));
            }
            return REDIRECT_CODE;
        } break;
    }
    return 0;
}

/*
Takes a url and adds the links in its page to the queue
*/
void fetch_page_add_hrefs(char *url, vector *queueSites) {
    int code;
    Authorization auth = create_auth(NULL);
    do {
        Http_Response response = fetch_page_url(url, &auth);
        code = handle_response(response, url, queueSites, &auth);
        free_http_response(response);
    } while (code == TRY_AGAIN_AGAIN);
}

void crawler(str url) {
    main_link = url;

    vector queueSites = create_vector_strings();
    char *fist_url = strdup(url);
    push_back(&queueSites, &fist_url);
    for (size_t i = 0; i < queueSites.size; i++) {
        char *url = at_str(&queueSites, i);
        printf("%s\n", url);
        fetch_page_add_hrefs(url, &queueSites);
    }

    free_vector(&queueSites);
}