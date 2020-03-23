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
    return strcmp(*str1, *str2) == 0;
}


void fetch_page_add_hrefs(char* url, vector* queueSites){
    char *host = get_host(url);

    Http_Response response = fetch_page_url(url);  
    vector links = get_links(response.body);
    free_http_response(response);
    
    map_inplace(&links, refactor_url, host, url);

    //filter out invalid responses
    filter(&links, is_valid_url);

    //filter out visited or plan to vist urls
    for (int i = 0; i < links.size; i++)
    {
        if (!vector_any(queueSites, at(&links, i), str_equal)){
            char *url = queueSites->copy_element(at(&links, i));
            push_back(queueSites, &url);
        }
    }
    
    free_vector(&links);
}

int main(int argc, char **argv){
    vector queueSites = create_vector_strings();
    if (argc < 2){
        puts("Parameters: <url>");
        exit(0);
    }
    char *fist_url = strdup(argv[1]);
    push_back(&queueSites, &fist_url);
    
    // char* url1 = strdup("http://www.johnnojohn.com/");
    // push_back(&queueSites, &url1);
    // char* url2 = strdup("http://www.ibdhost.com/");
    // push_back(&queueSites, &url2);
    // char* url3 = strdup("http://www.johnnojohn.com/style/style.css");
    // push_back(&queueSites, &url3);
 
    for (size_t i = 0; i < queueSites.size; i++){   
        char* url = at_str(&queueSites, i);
        printf("%s\n", url);
        fetch_page_add_hrefs(url, &queueSites);
    }
    
    free_vector(&queueSites);
    return 0;
}