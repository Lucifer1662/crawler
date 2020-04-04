#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "fetch_page/fetch_page.h"
#include "filter/filter.h"
#include "html/get_links.h"
#include "uri/uri.h"
#include "vector/vector.h"
#include "vector/vector_strings.h"
#include "crawler.h"

#define OK_STATUS 200
#define NOT_FOUND_STATUS 404
#define GONE_STATUS 410
#define URI_TOO_LONG_STATUS 414
#define SERVICE_UNAVAILABLE_STATUS 503
#define GATEWAY_TIMEOUT_STATUS 504
#define MOVED_PERMENTLY_STATUS 301
#define UNATHORIZED_STATUS 401

#define REDIRECT_CODE 2
#define TRY_AGAIN_AGAIN 1
#define FINISHED_CODE 0
#define USERNAME "lmhawkins"
#define PASSWORD "password"
#define AUTH_64_ENCODED "bG1oYXdraW5zOnBhc3N3b3Jk"

int str_equal(str str1, str str2) { return strcmp(str1, str2) == 0; }

int is_str_in(vector *vec, str str) {
    any_return(vec, at_str, str_equal, str);
    return 0;
}

void add_links_from_html(Http_Response response, char *url,
                         vector *queueSites) {
    char *host = get_host(url);
    vector links = get_links(response.body);

    map_inplace(&links, refactor_url, host, url);

    // filter out invalid responses
    filter_inplace(&links, at_str, is_valid_url);
    // filter out visited or plan to vist urls
    for (int i = 0; i < links.size; i++) {
        if (!is_str_in(queueSites, at_str(&links, i))) {
            char *url = queueSites->copy_element(at(&links, i));
            push_back(queueSites, &url);
        }
    }
    free_vector(&links);
    free(host);
}



int handle_response(Http_Response response, char *url, vector *queueSites, Authorization* auth) {
    if (response.body != NULL)
        add_links_from_html(response, url, queueSites);

    switch (response.header.statusCode) {
        case OK_STATUS:
        case NOT_FOUND_STATUS:
        case URI_TOO_LONG_STATUS:
        case GONE_STATUS:
            return FINISHED_CODE;
        
        case UNATHORIZED_STATUS:
            if(auth->userid_password_64 == NULL){
                *auth = create_auth(AUTH_64_ENCODED);
            }else{
                //we have already tried and our credentials are wrong
                return FINISHED_CODE;
            }
        case SERVICE_UNAVAILABLE_STATUS:
        case GATEWAY_TIMEOUT_STATUS:
            return TRY_AGAIN_AGAIN;

        case MOVED_PERMENTLY_STATUS: {
            printf("MOVED PERMENTLY %s\n", response.header.redirect);
            char *redirect = response.header.redirect;
            if (!is_str_in(queueSites, redirect)){
                push_back_str(queueSites, strdup(redirect));
            }
            return REDIRECT_CODE;
        } break;
    }
    return 0;
}

void fetch_page_add_hrefs(char *url, vector *queueSites) {
    int code;
    Authorization auth = create_auth(NULL);
    do {
        Http_Response response = fetch_page_url(url, &auth);
        code = handle_response(response, url, queueSites, &auth);
        free_http_response(response);
    } while (code == TRY_AGAIN_AGAIN);
}



void crawler(str url){
    main_link = url;

    vector queueSites = create_vector_strings();
    char *fist_url = strdup(url);
    push_back(&queueSites, &fist_url);
    for (size_t i = 0; i < queueSites.size; i++) {
        char *url = at_str(&queueSites, i);
        printf("%s\n", url);
        fetch_page_add_hrefs(url, &queueSites);
    }

    free_vector(&queueSites);
}