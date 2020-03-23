#include "fetch_page.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../uri/uri.h"

Http_Response_Header create_http_response_header(char *header_str)
{
    Http_Response_Header header;
    header.statusCode = 200;
    return header;
}

Http_Response create_http_response(char *response_str)
{
    Http_Response response;
    int header_length = strstr(response_str, "\r\n\r\n") - response_str;

    char *header = strndup(response_str, header_length);
    response.header = create_http_response_header(header);
    free(header);

    response_str = &response_str[header_length + strlen("\r\n\r\n")];

    response.body = strdup(response_str);
    return response;
}

void free_http_response(Http_Response response){
    free(response.body);
}

Http_Response fetch_page_url(char *url)
{
    Uri uri = create_uri(url);
    return fetch_page_uri(uri);
}

Http_Response fetch_page_uri(Uri uri)
{
    struct addrinfo hints, *res;
    int sockfd;

    char buf[2056];
    memset(buf, 0, sizeof(buf));
    int byte_count;

    //get host info, make socket and connect it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(uri.host, "80", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    //char *header = "GET /index.html HTTP/1.1\r\nHost: www.ibdhost.com\r\n\r\n";
    char *header_format =
        "GET %s HTTP/1.1\r\n"
        "Host: www.ibdhost.com\r\n"
        "Connection: keep-alive\r\n"
        "Cache-Control: max-age=0\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 Safari/537.36\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Accept-Language: en-AU,en-GB;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n";
    char header[1000];
    sprintf(header, header_format, uri.path);
    send(sockfd, header, strlen(header), 0);
    char *page = strdup("");
    while ((byte_count = read(sockfd, buf, sizeof(buf)-1)) > 0){
        page = realloc(page, strlen(page) + byte_count + 1);
        strcat(page, buf);
        memset(buf, 0, sizeof(buf));
    }

    Http_Response response = create_http_response(page);
    free(page);
    close(sockfd);

    return response;
}