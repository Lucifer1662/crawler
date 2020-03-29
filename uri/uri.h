#ifndef URI
#define URI


struct Uri_t{
    char* host;
    char* path;
};
typedef struct Uri_t Uri;

void delete_uri(Uri uri);

char* get_path(char* url);
char* get_host(char* url);

Uri create_uri(char* url);

void refactor_url(char** url, char* host, char* host_url);
int starts_with(char *start, char *str);
int is_relative_path(char *url);
int is_valid_url(char *link);
#endif