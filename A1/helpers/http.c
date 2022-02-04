#include "*.h"

struct http_response* init_response(){
    struct http_response *response = malloc(sizeof(struct http_response*)); 
    if(response == NULL) return NULL;

    response->header = malloc(sizeof(struct http_header*));
    if(response == NULL) return NULL;

    response->header->version = malloc(sizeof(int));

    response->body = malloc(sizeof(struct http_body*));
    if(response == NULL) return NULL;

    return response;
}

// Check header method, not too sure what else to add here
int validate_request(char** rq, int num_tokens){
    if(num_tokens != 3) return 0;
    
    if(strncmp(rq[0], "GET", 3) != 0){
        return 0;
    }if(strncmp(rq[1], "/", 1) != 0){
        return 0;
    }if(strncmp(rq[2], "HTTP/1.0", strlen("HTTP/1.0")) != 0 && 
        strncmp(rq[2], "HTTP/1.1", strlen("HTTP/1.1")) != 0){
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
        // treat as text data???
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

// As stated in piazza (link), we are using the date in the format of
// <day-name>, <day> <month> <year> as the etag
int match_etag(char* header_info, struct stat st){
    char file_mtime[45] = {0};
    struct tm buff;
    struct tm* ptime = gmtime_r(&(st.st_mtime), &buff);
    strftime(file_mtime, sizeof(file_mtime), "%a, %d %b %Y", ptime);
        
    if(strncmp(file_mtime, header_info, strlen(file_mtime)) == 0){
        return 1;
    }
    return 0;
}


int parse_headers(char** hreqs, int num_hr, struct stat st, struct http_response* rsp){
    for(int i = 1; i < num_hr; i++){
        int y = 0;
        char** header_info = split_string_char(hreqs[i], ':', 1, &y);
        char* header = header_info[0];
        char* info = header_info[1];

        if(strncmp(header, "If-Match", strlen("If-Match")) == 0){
            int matched = match_etag(info, st);

            if(matched){
                return 200;
            }else{
                return 404;
            }

        }else if(strncmp(header, "If-None-Match", strlen("If-None-Match")) == 0){
            int matched = match_etag(info, st);

            if(!matched){
                return 200;
            }else{
                return 404;
            }

        }else if(strncmp(header, "If-Modified-Since", strlen("If-Modified-Since")) == 0){
            time_t request_time = get_time(info, "%a, %d %b %Y %T GMT");

            if(difftime(request_time, st.st_mtime) < 0){
                return 200;
            }else{
                return 304;
            }

        }else if(strncmp(header, "If-UnModified-Since", strlen("If-UnModified-Since")) == 0){
            time_t request_time = get_time(info, "%a, %d %b %Y %T GMT");

            if(difftime(request_time, st.st_mtime) > 0){
                return 200;
            }else{
                return 412;
            }
        }
    }
    
    return 200;
}

// Returns the http response header depending on the status 
char* generate_header(struct http_header* header){
    char* header_rsp = malloc(sizeof(char) * 1024);
    char status_msg[128];

    if(header->status == 200){
        sprintf(header_rsp,
                "%s 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %d\r\n\r\n",
                header->version, header->content_type, (int) header->content_length);

        return header_rsp;
    }else{
        if(header->status == 400){
            sprintf(status_msg, "400 Bad Request");

        }else if(header->status == 304){
            sprintf(status_msg, "304 Not Modified");

        }else if(header->status == 404){
            sprintf(status_msg, "404 Not Found"); 

        }else if(header->status == 412){
            sprintf(status_msg, "412 Precondition Failed");

        }else if(header->status == 500){
            sprintf(status_msg, "500 Internal Server Error");

        }else if(header->status == 501){
            sprintf(status_msg, "501 Not Implemented");

        }else return NULL;

        sprintf(header_rsp, "%s %s\r\n\r\n", header->version, status_msg);
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
        printf("BAD HEADER_REQS...\n");
        return response;
    }

    char** request_line = split_string_char(header_reqs[0], ' ', -1, &num_rl);

    if(request_line == NULL){
        response->header->status = 400;
        printf("BAD REQUEST LINE...\n");
        return response;
    }
    if(!validate_request(request_line, num_rl)){
        response->header->status = 400;
        printf("BAD VALIDATE REQ...\n");
        return response;
    }

    response->header->version = request_line[2];

    // Get file starting from root directory...
    char* filepath = malloc(sizeof(char) * (strlen(http_root_path) + strlen(request_line[1]) + 2));
    //snprintf(filepath, strlen(request_line[1]) + 2, ".%s", request_line[1]);
    strcpy(filepath, http_root_path);
    strncat(filepath, request_line[1], strlen(request_line[1]));

    printf("Client requested: %s\n", filepath);

    // Checks if the file exists
    if(access(filepath, F_OK) == 0){

        struct stat st;
        stat(filepath, &st);

        if (!S_ISREG(st.st_mode)){
            response->header->status = 400;
            free(header_reqs);
            free(request_line);
            free(filepath);
            printf("This was not a file...\n");
            return response;
        }

        if(!set_mime_type(filepath, response)){
            response->header->status = 501;
            free(header_reqs);
            free(request_line);
            free(filepath);
            printf("This was not a valid mime type...\n");
            return response;
        };

        stat(filepath, &st);

        response->header->status = parse_headers(header_reqs, num_hr, st, response);
        printf("Response status: %d\n", response->header->status);
        response->header->content_length = st.st_size;
        response->body->fp = fopen(filepath, "r");
        
    }else{
        response->header->status = 400;
        printf("Could not get file...\n");
    }

    free(header_reqs);
    free(request_line);
    free(filepath);

    return response;
}
