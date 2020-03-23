#include "uri.h"
#include "string.h"
#include "stdlib.h"
#define START_OF_HOST 7
#define HTTP_STR "http://"

Uri create_uri(char* url){
    char* init_url = url;
    Uri uri;
    url = &url[START_OF_HOST];
    char* slash_ptr = strchr(url, '/');
    if(slash_ptr == NULL){
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

char* get_host(char* url){
    url = &url[START_OF_HOST];
    char* slash_ptr = strchr(url, '/');
    if(slash_ptr == NULL)
        return strdup(url);
    int host_length = slash_ptr - url;

    Uri uri;
    return strndup(url, host_length);
}

char* get_path(char* url){
    if(starts_with(".", url))
        return url;
    if(starts_with(HTTP_STR, url))
        url = &url[START_OF_HOST];

    char* slash_ptr = strchr(url, '/');
    if(slash_ptr == NULL){
        return NULL;
    }
    int host_length = slash_ptr - url;
    url = &url[host_length];
    return url;
}




void delete_uri(Uri uri){
    free(uri.host);
    free(uri.path);
}




char* refactor_url(char** url_ptr, char* host, char* host_url){
    if(!starts_with(HTTP_STR, *url_ptr)){
        char* relative = starts_with("/", *url_ptr) ? host : host_url;

        int newLength = strlen(HTTP_STR)
        + strlen(relative) + strlen(*url_ptr) + 1;

        char* newUrl = malloc(newLength*sizeof(char));

        sprintf(newUrl, "%s%s%s", HTTP_STR, relative, *url_ptr);

        free(*url_ptr);
        *url_ptr = newUrl;
    }

    if(get_path(*url_ptr) == NULL){
        *url_ptr = realloc(*url_ptr, strlen(*url_ptr)*sizeof(char)+2);
        sprintf(*url_ptr, "%s/", *url_ptr);
    }
}

int starts_with(char *start, char *str)
{
    if (strlen(start) > strlen(str)){
        return 0;
    }
    for (int i = 0; start[i] != 0 && str[i] != 0; i++){
        if (start[i] != str[i])
            return 0;
    }
    return 1;
}

int is_relative_path(char *url)
{
    return strstr(url, "./") != NULL ||  strstr(url, "..") != NULL;
}

int is_valid_url(char **link)
{
    char *l = *link;
    return !is_relative_path(l);
}
