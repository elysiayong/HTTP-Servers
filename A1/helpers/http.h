#include "utils.h"
#include <time.h>

struct http_response{
    struct http_header* header;
    struct http_body* body;
};

struct http_header{
    char* content_type;
    char* version; 
    int status;
    int content_length;
    char* connection_type;
};

struct http_body{
    FILE* fp;
};

struct http_response* type_response(char*);
char* generate_header(struct http_header*);