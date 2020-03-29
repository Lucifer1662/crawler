#include "../vector/vector_strings.h"
#include "assert.h"
#include "get_links.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define SIZE 2048
int main(int argc, char** argv) {
    if (argc < 3) {
        puts("Parameters: <html_file> <answer_file> ");
        exit(0);
    }
    char* html_file = argv[1];
    char* answer_file = argv[2];
    vector v;
    {
        char read_el[SIZE];
        memset(read_el, 0, sizeof read_el);
        void* fp = fopen(html_file, "r");

        if (fp == NULL) {
            printf("File Opening Error!!");
        }
        char* html = strdup("");
        while (fgets(read_el, SIZE, fp) != NULL) {
            html = realloc(html, strlen(html) + strlen(read_el) + 1);
            strcat(html, read_el);
            memset(read_el, 0, sizeof read_el);
        }
        v = get_links(html);
        fclose(fp);
    }
    {
        char read_el[SIZE];
        memset(read_el, 0, sizeof read_el);
        void* fp = fopen(answer_file, "r");

        if (fp == NULL) {
            printf("File Opening Error!!");
        }
        int i = 0;
        while (fgets(read_el, SIZE, fp) != NULL) {
            read_el[strlen(read_el) - 2] = 0;
            printf("%s = %s\n", read_el, at_str(&v, i));
            assert(strcmp(read_el, at_str(&v, i)) == 0);
            memset(read_el, 0, sizeof read_el);
            i++;
        }
        fclose(fp);
    }

    free_vector(&v);

    return 0;
}