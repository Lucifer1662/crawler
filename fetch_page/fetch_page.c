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
#include "../str_util/str_util.h"



#define AUTH_STR "authorization: Basic "

#define BUFFER_SIZE 2056
#define HEADER_FORMAT                                              \
    "GET %s HTTP/1.1\r\n"                                          \
    "Host: %s\r\n"                                                 \
    "%s"                                                           \
    "Connection: keep-alive\r\n"                                   \
    "Cache-Control: max-age=0\r\n"                                 \
    "Upgrade-Insecure-Requests: 1\r\n"                             \
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "       \
    "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 " \
    "Safari/537.36\r\n"                                            \
    "Accept: "                                                     \
    "text/html,application/xhtml+xml,application/xml;q=0.9,image/" \
    "webp,image/apng,*/*;q=0.8,application/"                       \
    "signed-exchange;v=b3;q=0.9\r\n"                               \
    "Accept-Encoding: \r\n"                                        \
    "Accept-Language: en-AU,en-GB;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n"
#define HTTP_PORT_STR "80"
#define MAX_HEADER_LENGTH 1000




Http_Response fetch_page_url(char* url, Authorization* auth) {
    Uri uri = create_uri(url);
    return fetch_page_uri(uri, auth);
}

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
    while (response.body == NULL) {
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
        } else {
            response.body =
                read_page_length(socket, header.content_length, page_remnant);
        }

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