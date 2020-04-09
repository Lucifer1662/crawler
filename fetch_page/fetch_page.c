#include "fetch_page.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../str_util/str_util.h"
#include "../uri/uri.h"

#define AUTH_STR "authorization: Basic "

#define BUFFER_SIZE 2056
#define HEADER_FORMAT                  \
    "GET %s HTTP/1.1\r\n"              \
    "Host: %s\r\n"                     \
    "%s"                               \
    "User-Agent: lmhawkins"            \
    "\r\n\r\n"
#define HTTP_PORT_STR "80"
#define MAX_HEADER_LENGTH 1000
#define MAX_REFETCH 3

Http_Response fetch_page_url(char* url, Authorization* auth) {
    printf("Fetching: %s\n", url);
    Uri uri = create_uri(url);
    Http_Response resp = fetch_page_uri(uri, auth);
    printf("Retived Body:%d", resp.body != NULL);
    return resp;
}

/*
Reads from the sockfd until the end of the stream
 - page_remnant is concatinated to the beginning
 - returns a char* of the page in the heap
*/
char* read_page_till_end(int sockfd, char* page_remnant) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, sizeof(buf));
    char* page = strdup(page_remnant);
    int bytes_read;
    while ((bytes_read = read(sockfd, buf, sizeof(buf) - 1)) > 0) {
        int newLength = 1;
        newLength += strlen(page);
        newLength += strlen(buf);
        page = realloc(page, sizeof(char) * newLength);
        strcat(page, buf);
        memset(buf, 0, sizeof(buf));
    }
    return page;
}

/*
reads in a page from a socket taking and only reading until
length or until the stream ends
 - if the stream is struncated then return NULL
 - else returns the page
*/
char* read_page_length(int sockfd, int length, char* page_remnant) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, sizeof(buf));
    int byte_count = 0;

    char* page = malloc(length + 1);
    memset(page, 0, sizeof(char) * (length + 1));
    strcat(page, page_remnant);
    byte_count += strlen(page_remnant);

    while (byte_count < length) {
        int bytes_read = read(sockfd, buf, sizeof(buf) - 1);
        byte_count += bytes_read;
        if (bytes_read == 0 && byte_count != length) {
            free(page);
            return NULL;
        }

        strcat(page, buf);
        memset(buf, 0, sizeof(buf));
    }
    return page;
}

/*
Reads the header from the socket
 - it may read in part of the page, this is
 return in the page_remnant in the heap

*/
char* read_header(int sockfd, char** page_remnant) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, sizeof(buf));

    char* response_header = strdup("");
    char* header_end;
    int byte_count;
    // get header
    while ((header_end = strstr(response_header, HEADER_END)) == NULL &&
           (byte_count = read(sockfd, buf, sizeof(buf) - 1)) > 0) {
        int new_length = strlen(response_header) + byte_count + 1;
        response_header = realloc(response_header, new_length);
        strcat(response_header, buf);
        memset(buf, 0, sizeof(buf));
    }

    header_end += strlen(HEADER_END);
    *page_remnant = strdup(header_end);

    char* new_response_header =
        strndup(response_header, header_end - response_header);
    header_names_to_lower(new_response_header);
    free(response_header);
    return new_response_header;
}

char* create_athorization_line(Authorization* auth) {
    return concat_create(3, AUTH_STR, auth->userid_password_64, LINE_END);
}

/*
Sends a http get with the uri and auth
 - returns a socket to that page
*/
int send_http_get(Uri uri, Authorization* auth) {
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(uri.host, HTTP_PORT_STR, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    
    char* auth_line = strdup("");
    if (auth != NULL && auth->userid_password_64 != NULL) {
        auth_line = create_athorization_line(auth);
    }

    char get_header[MAX_HEADER_LENGTH];
    sprintf(get_header, HEADER_FORMAT, uri.path, uri.host, auth_line);
    send(sockfd, get_header, strlen(get_header), 0);
    free(auth_line);
    return sockfd;
}

Http_Response fetch_page_uri(Uri uri, Authorization* auth) {
    Http_Response response;
    response.body = NULL;
    int refetch_count = 0;
    while (response.body == NULL && refetch_count < MAX_REFETCH) {
        int socket = send_http_get(uri, auth);

        char* page_remnant;
        char* response_header = read_header(socket, &page_remnant);

        Http_Response_Header header =
            create_http_response_header(response_header);

        response.header = header;
        if (!header.contains_html_text) {
            response.body = NULL;
            free(page_remnant);
            free(response_header);
            close(socket);
            return response;
        }

        if (header.content_length == NO_HEADER_INT) {
            response.body = read_page_till_end(socket, page_remnant);
            printf("Reading NO Length\n");
        } else {
            
            response.body =
                read_page_length(socket, header.content_length, page_remnant);
            printf("Reading page length %d %d\n", header.content_length, strlen(response.body));
        }

        refetch_count ++;

        free(page_remnant);
        free(response_header);
        close(socket);
    }

    return response;
}

Authorization create_auth(char* userid_password_64) {
    Authorization auth;
    auth.userid_password_64 = userid_password_64;
    return auth;
}