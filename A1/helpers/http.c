#include "*.h"

struct http_response* init_response(){
    struct http_response *response = malloc(sizeof(struct http_response*)); 
    if(response == NULL) return NULL;

    response->header = malloc(sizeof(struct http_header*));
    if(response == NULL) return NULL;

    response->body = malloc(sizeof(struct http_body*));
    if(response == NULL) return NULL;

    return response;
}

// Check header method, not too sure what else to add here
int validate_request(char** rq){
    if(strncmp(rq[0], "GET", 3) != 0){
        return 0;
    }if(strncmp(rq[1], "/", 1) != 0){
        return 0;
    }
    return 1;
}

// NOTE: The filepath should be sanitized before sending to this function
//       Idk whether it makes more sense to handle sanitization in the function or not we'll see
// EXAMPLE SEGFAULT: filepath = "./foo/bar.txt/"
// Because of the trailing '/', we will go out of bounds and read illegal memory
int set_mime_type(char* filepath, struct http_response* rsp){
    char* filename = strrchr(filepath, '/');
    filename++;
    char* ext = strrchr(filename, '.');

    if(!ext){
        // treat as text data
        rsp->header->content_type = "text/plain";
        return 1;
    }else{
        ext++;
        if(strncmp(ext, "txt", 3) == 0){
            rsp->header->content_type = "text/plain";
            return 1;
        }else if(strncmp(ext, "html", 4) == 0){
            rsp->header->content_type = "text/html";
            return 1;
        }else if(strncmp(ext, "css", 3) == 0){
            rsp->header->content_type = "text/css";
            return 1;
        }else if(strncmp(ext, "js", 2) == 0){
            rsp->header->content_type = "text/javascript";
            return 1;
        }else if(strncmp(ext, "jpeg", 4) == 0){
            rsp->header->content_type = "image/jpeg";
            return 1;
        }else if(strncmp(ext, "jpg", 3) == 0){
            rsp->header->content_type = "image/jpg";
            return 1;
        }
    }

    return 0;
}

// TODO: check what the conditionals mean, likely need to modify parameters
// to include filepath to check file information(?)
void parse_headers(char** hreqs, int num_hr, struct http_response* rsp){
    
    for(int i = 1; i < num_hr; i++){
        int y = 0;
        char** header_info = split_string_char(hreqs[i], ':', &y);
        char* header = header_info[0];
        // char* info = header_info[1];

        // Check for conditionals, set rsp and return accordingly
        if(strncmp(header, "If-Match", strlen("If-Match")) == 0){
            
        }if(strncmp(header, "If-None-Match", strlen("If-None-Match")) == 0){
            
        }if(strncmp(header, "If-Modified-Since", strlen("If-Modified-Since")) == 0){
            
        }if(strncmp(header, "If-UnModified-Since", strlen("If-UnModified-Since")) == 0){
            
        }
    }
}

// Returns the http response header depending on the status 
char* generate_header(struct http_header* header){
    char* header_rsp = malloc(sizeof(char) * 1024);
    char status_msg[128];

    if(header->status == 200){
        sprintf(header_rsp,
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %d\r\n\r\n",
                header->content_type, (int) header->content_length);

        return header_rsp;
    }else{
        if(header->status == 400){
            sprintf(status_msg, "400 Bad Request");

        }else if(header->status == 404){
            sprintf(status_msg, "404 Not Found"); 

        }else if(header->status == 304){
            sprintf(status_msg, "304 Not Modified"); 

        }else if(header->status == 500){
            sprintf(status_msg, "500 Internal Server Error");

        }else if(header->status == 501){
            sprintf(status_msg, "501 Not Implemented");

        }else return NULL;

        sprintf(header_rsp, "HTTP/1.0 %s\r\n\r\n", status_msg);
        return header_rsp;
    }
    return NULL;
}

// Handles request appropriately and generates an http response
// TODO: Messy looking becauase of error checks will need to polish
struct http_response* type_response(char* line){
    struct http_response* response = init_response();
    char* request = line;
    int num_hr, num_rl = 0;
    char** header_reqs = split_string(request, "\r\n", &num_hr);

    if(header_reqs == NULL){
        response->header->status = 400;
        return response;
    }

    char** request_line = split_string_char(header_reqs[0], ' ', &num_rl);

    if(request_line == NULL){
        response->header->status = 400;
        return response;
    }
    if(!validate_request(request_line)){
        response->header->status = 400;
        return response;
    }

    parse_headers(header_reqs, num_hr, response);

    // TODO: Handle case for relative & absolute path 
    // TODO: Handle conditional get request status or wtv
    // NOTE: Current implementation handles relative 
    char* filepath = malloc(sizeof(char) * (strlen(http_root_path) + strlen(request_line[1]) + 2));
    //snprintf(filepath, strlen(request_line[1]) + 2, ".%s", request_line[1]);
    strcpy(filepath, http_root_path);
    strncat(filepath, request_line[1], strlen(request_line[1]));

    // Checks if the file exists
    if(access(filepath, F_OK) == 0){

        struct stat st;
        stat(filepath, &st);

        if (!S_ISREG(st.st_mode)){
            response->header->status = 400;
            free(header_reqs);
            free(request_line);
            free(filepath);
            return response;
        }

        if(!set_mime_type(filepath, response)){
            response->header->status = 400;
            free(header_reqs);
            free(request_line);
            free(filepath);
            return response;
        };

        response->header->content_length = st.st_size;
        response->body->fp = fopen(filepath, "r");
        response->header->status = 200;
        
    }else{
        // TODO: I forgor which status code lol pepehands
        response->header->status = 400;
    }

    free(header_reqs);
    free(request_line);
    free(filepath);

    return response;
}
