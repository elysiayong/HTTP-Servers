#include "utils.h"

struct http_response{
    struct http_header* header;
    struct http_body* body;
};

struct http_header{
    char* content_type;
    char* version; 
    int status;
    int content_length;
    
    char* connection;
};

struct http_body{
    FILE* fp;
};

struct http_response* init_response();
void parse_request_modify_response(char**, int, struct http_response*);
char* response_header_to_str(struct http_header*);