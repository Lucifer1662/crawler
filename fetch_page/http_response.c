#include "http_response.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include "../str_util/str_util.h"

#define STATUS_HEADER "HTTP/1.1 "
#define REDIRECT_LINK "location: "
#define HEADER_NAME_END ':'
#define FIRST_CHAR_LINE_END '\r'
#define SPACE ' '
#define TEXT_OR_HTML "text/html"
#define CONTENT_TYPE "content-type: "
#define CONTENT_LENGTH "content-length: "

int get_status_from_header(char* header_str) {
    char* status_str = strstr(header_str, STATUS_HEADER);
    int res = NO_HEADER_STATUS;
    if (status_str != NULL) {
        status_str += strlen(STATUS_HEADER);
        char* end = strchr(status_str, SPACE);
        *end = 0;
        res = atoi(status_str);
        *end = ' ';
    }
    return res;
}

int get_int_from_header(char* header_str, char* name) {
    char* status_str = strstr(header_str, name);
    int res = NO_HEADER_INT;
    if (status_str != NULL) {
        status_str += strlen(name);
        char* end = strchr(status_str, FIRST_CHAR_LINE_END);
        *end = 0;
        res = atoi(status_str);
        *end = FIRST_CHAR_LINE_END;
    }
    return res;
}

char* get_str_from_header(char* header_str, char* name) {
    char* start_line = strstr(header_str, name);
    char* res = NO_HEADER_STR;
    if (start_line != NULL) {
        start_line += strlen(name);
        char* end = strchr(start_line, FIRST_CHAR_LINE_END);
        *end = 0;
        res = strdup(start_line);
        *end = FIRST_CHAR_LINE_END;
    }
    return res;
}

int header_line_contains(char* header_str, char* name, char* value) {
    char* start_line = strstr(header_str, name);
    int res = NO_HEADER_STR;
    if (start_line != NULL) {
        start_line += strlen(name);
        char* end = strchr(start_line, FIRST_CHAR_LINE_END);
        *end = 0;
        res = (strstr(start_line, value) != NULL);
        *end = FIRST_CHAR_LINE_END;
    }
    return res;
}

Http_Response_Header create_http_response_header(char* header_str) {
    Http_Response_Header header;
    header.statusCode = get_status_from_header(header_str);
    header.redirect = get_str_from_header(header_str, REDIRECT_LINK);
    header.content_length = get_int_from_header(header_str, CONTENT_LENGTH);
    header.contains_html_text =
        header_line_contains(header_str, CONTENT_TYPE, TEXT_OR_HTML);
    return header;
}

Http_Response create_http_response(char* header_str, char* body_str) {
    Http_Response response;
    response.header = create_http_response_header(header_str);
    response.body = strdup(body_str);
    return response;
}

void free_http_response_header(Http_Response_Header header) {
    free(header.redirect);
}

void free_http_response(Http_Response response) {
    free_http_response_header(response.header);
    free(response.body);
}



void header_names_to_lower(char* header) {
    char* end_name;
    header = strstr(header, LINE_END);
    while (header && *header) {
        end_name = strchr(header, HEADER_NAME_END);
        if (end_name != NULL) {
            *end_name = 0;
            str_to_lower(header);
            *end_name = HEADER_NAME_END;
            header = end_name;
        }
        header = strstr(header, LINE_END) + strlen(LINE_END);
    }
}