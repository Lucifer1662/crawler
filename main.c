#include <stdio.h>
#include <stdlib.h>
#include "crawler.h"



int main(int argc, char **argv) {
    if (argc < 2) {
        puts("Parameters: <url>");
        exit(0);
    }
    crawler(argv[1]);
    return 0;
}