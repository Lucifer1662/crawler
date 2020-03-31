#include "uri.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "../crawler.h"
#define START_OF_HOST 7
#define HTTP_STR "http://"

Uri create_uri(char* url) {
    char* init_url = url;
    Uri uri;
    url = &url[START_OF_HOST];
    char* slash_ptr = strchr(url, '/');
    if (slash_ptr == NULL) {
        uri.host = strdup(init_url);
        uri.path = strdup("/");
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
    char* slash_ptr = strchr(url, '/');
    if (slash_ptr == NULL) return strdup(url);
    int host_length = slash_ptr - url;

    return strndup(url, host_length);
}

char* get_path(char* url) {
    if (starts_with(".", url)) return url;
    if (starts_with(HTTP_STR, url)) url = &url[START_OF_HOST];

    char* slash_ptr = strchr(url, '/');
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
    host_url += strlen(HTTP_STR);
    if (!starts_with(HTTP_STR, *url_ptr)) {
        char* relative;
        if(starts_with("/", *url_ptr)){
            relative = strdup(host);
        }else{
            char* last_slash = strrchr(host_url, '/');
            if(last_slash == NULL){
                relative = strdup(host_url);
            }else
            {
                relative = strndup(host_url, last_slash - host_url + 1);
            }
        }

        int newLength =
            strlen(HTTP_STR) + strlen(relative) + strlen(*url_ptr) + 1;

        char* newUrl = malloc(newLength * sizeof(char));

        sprintf(newUrl, "%s%s%s", HTTP_STR, relative, *url_ptr);

        free(*url_ptr);
        free(relative);
        *url_ptr = newUrl;
    }

    if (get_path(*url_ptr) == NULL) {
        *url_ptr = realloc(*url_ptr, strlen(*url_ptr) * sizeof(char) + 2);
        strcat(*url_ptr, "/");
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
    return strstr(url, "./") != NULL || strstr(url, "..") != NULL;
}

char* chop_off_head(char* host){
    char* tail = strchr(host, '.');
    if(tail == NULL)
        tail = host+strlen(host);
    return tail;
}

int same_tail_components(char* link1, char* link2){
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
    return !is_relative_path(link) && 
    strchr(link, '#') == NULL &&
           strchr(link, '%') == NULL &&
           same_tail_components(link, main_link)
           ;
}
