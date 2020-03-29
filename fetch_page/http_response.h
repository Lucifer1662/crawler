#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#define HEADER_END "\r\n\r\n"
#define LINE_END "\r\n"
#define NO_HEADER_INT -1
#define NO_HEADER_STATUS -1
#define NO_HEADER_STR 0

struct Http_Response_Header_t {
    int statusCode;
    char* redirect;
    int contains_html_text;
    int content_length;
};
typedef struct Http_Response_Header_t Http_Response_Header;

struct Http_Response_T {
    Http_Response_Header header;
    char* body;
};
typedef struct Http_Response_T Http_Response;

Http_Response_Header create_http_response_header(char* header_str);
Http_Response create_http_response(char* header_str, char* body_str);
void free_http_response_header(Http_Response_Header header);
void free_http_response(Http_Response response);
void header_names_to_lower(char* header);

#endif