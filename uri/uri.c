#include "uri.h"

#include "../crawler.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../str_util/str_util.h"
#define START_OF_HOST 7
#define HTTP_PROTOCOL_STR "http://"
#define HTTP_STR "http:"
#define SLASH_CHAR '/'
#define SLASH_STR "/"
#define DOUBLE_SLASH_STR "//"
#define HOST_COMPONENT_SEPERATOR_STR "."
#define RELATIVE_SLASH "./"
#define UP_ONE_DIRECTORY ".."
#define HOST_COMPONENT_SEPERATOR '.'
#define FRAGMENT_CHAR '#'
#define ENCODED_CHAR '%'

Uri create_uri(char* url) {
    char* init_url = url;
    Uri uri;
    url = &url[START_OF_HOST];
    char* slash_ptr = strchr(url, SLASH_CHAR);
    if (slash_ptr == NULL) {
        uri.host = strdup(init_url);
        uri.path = strdup(SLASH_STR);
        return uri;
    }
    int host_length = slash_ptr - url;
    uri.host = strndup(url, host_length);
    url = &url[host_length];
    uri.path = strdup(url);
    return uri;
}

char* get_host(char* url) {
    url = &url[START_OF_HOST];
    char* slash_ptr = strchr(url, SLASH_CHAR);
    if (slash_ptr == NULL) return strdup(url);
    int host_length = slash_ptr - url;

    return strndup(url, host_length);
}

char* get_path(char* url) {
    if (starts_with(HOST_COMPONENT_SEPERATOR_STR, url)) return url;
    if (starts_with(HTTP_PROTOCOL_STR, url)) url = &url[START_OF_HOST];

    char* slash_ptr = strchr(url, SLASH_CHAR);
    if (slash_ptr == NULL) {
        return NULL;
    }
    int host_length = slash_ptr - url;
    url = &url[host_length];
    return url;
}

void delete_uri(Uri uri) {
    free(uri.host);
    free(uri.path);
}

void refactor_url(char** url_ptr, char* host, char* host_url) {
    printf("Refactor: %s\n", *url_ptr);
    host_url += strlen(HTTP_PROTOCOL_STR);
    if (!starts_with(HTTP_PROTOCOL_STR, *url_ptr)) {
        char* newUrl;
        if(starts_with(DOUBLE_SLASH_STR, *url_ptr)){
            newUrl = concat_create(2, HTTP_STR, *host_url);
        }
        else if (starts_with(SLASH_STR, *url_ptr)) {
            //relative = strdup(host);
            newUrl = concat_create(3, HTTP_PROTOCOL_STR, host, *url_ptr);
        } else {
            char* last_slash = strrchr(host_url, SLASH_CHAR);
            if (last_slash == NULL) {
                //relative = strdup(host_url);
                newUrl = concat_create(3, HTTP_PROTOCOL_STR, host_url, *url_ptr);
            } else {
                char* relative = strndup(host_url, last_slash - host_url + 1);
                newUrl = concat_create(3, HTTP_PROTOCOL_STR, host_url, *url_ptr);
                free(relative);
            }
        }

        //int newLength =
        //    strlen(HTTP_PROTOCOL_STR) + strlen(relative) + strlen(*url_ptr) + 1;

        //newUrl = malloc(newLength * sizeof(char));

        //sprintf(newUrl, "%s%s%s", HTTP_PROTOCOL_STR, relative, *url_ptr);

        free(*url_ptr);
        //free(relative);
        *url_ptr = newUrl;
    }

    if (get_path(*url_ptr) == NULL) {
        *url_ptr = realloc(*url_ptr, strlen(*url_ptr) * sizeof(char) + 2);
        strcat(*url_ptr, SLASH_STR);
    }
}

int starts_with(char* start, char* str) {
    if (strlen(start) > strlen(str)) {
        return 0;
    }
    for (int i = 0; start[i] != 0 && str[i] != 0; i++) {
        if (start[i] != str[i]) return 0;
    }
    return 1;
}

int is_relative_path(char* url) {
    return strstr(url, RELATIVE_SLASH) != NULL ||
           strstr(url, UP_ONE_DIRECTORY) != NULL;
}

char* chop_off_head(char* host) {
    char* tail = strchr(host, HOST_COMPONENT_SEPERATOR);
    if (tail == NULL) tail = host + strlen(host);
    return tail;
}

int same_tail_components(char* link1, char* link2) {
    char* host1 = get_host(link1);
    char* host2 = get_host(link2);
    char* tail_components1 = chop_off_head(host1);
    char* tail_components2 = chop_off_head(host2);
    int res = strcmp(tail_components1, tail_components2) == 0;
    free(host1);
    free(host2);
    return res;
}

int is_valid_url(char* link) {
    int kept =  !is_relative_path(link) && strchr(link, FRAGMENT_CHAR) == NULL &&
           strchr(link, ENCODED_CHAR) == NULL &&
           same_tail_components(link, main_link);
    printf("Kept:%d     %s\n", kept, link);
    return !is_relative_path(link) && strchr(link, FRAGMENT_CHAR) == NULL &&
           strchr(link, ENCODED_CHAR) == NULL &&
           same_tail_components(link, main_link);
}
