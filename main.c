#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fetch_page/fetch_page.h"
#include "html/get_links.h"
#include "filter/filter.h"
#include "uri/uri.h"
#include "vector/vector.h"
#include "vector/vector_strings.h"

int str_equal(char **str1, char **str2)
{
    //printf("%d cmp %s, %s\n",strcmp(*str1, *str2), *str1, *str2);
    return strcmp(*str1, *str2) == 0;
}


void fetch_page_add_hrefs(char* url, vector* sitesVisted, vector* queueSites){
    char *host = get_host(url);
    push_back_str(sitesVisted, url);

    Http_Response response = fetch_page_url(url);  
    vector links = get_links(response.body);

    
    map_inplace(&links, refactor_url, host);

    //filter out invalid responses
    filter(&links, is_valid_url);

    //filter out visited or plan to vist urls
    for (int i = 0; i < links.size; i++)
    {
        if (!vector_any(queueSites, at(&links, i), str_equal) &&
            !vector_any(sitesVisted, at(&links, i), str_equal)){
            char *url = queueSites->copy_element(at(&links, i));
            push_back(queueSites, &url);
        }
    }
    
    free_vector(&links);
}

int main(int argc, char **argv)
{

    vector sitesVisted = create_vector_strings();
    vector queueSites = create_vector_strings();
    char **visited;
    if (argc < 2)
    {
        puts("Parameters: <url>");
        exit(0);
    }

    char *url = strdup(argv[1]);

    
    fetch_page_add_hrefs(url, &sitesVisted, &queueSites);
    printf("%s\n", url);

    printf("Queue\n");
    for (int i = 0; i < queueSites.size; i++)
    {
        printf("%s\n", at_str(&queueSites, i));
    }

    free_vector(&queueSites);
    free_vector(&sitesVisted);
    return 0;
}