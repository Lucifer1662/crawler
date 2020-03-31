#include <stdio.h>
#include <stdlib.h>
#include "crawler.h"

#define PARAMETER_HELP "Parameters: <url>"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts(PARAMETER_HELP);
        exit(0);
    }
    crawler(argv[1]);
    return 0;
}