#include "*.h"

struct http_response* init_response() {

    struct http_response *response = malloc(sizeof(struct http_response*)); 

    response->header = malloc(sizeof(struct http_header*));
    response->header->content_type = malloc(sizeof(char*));
    response->header->version = malloc(sizeof(char*));
    // response->header->status;
    // response->header->content_length;
    response->header->connection = malloc(sizeof(char*));

    response->body = malloc(sizeof(struct http_body*));
    response->body->fp = malloc(sizeof(struct FILE*));

    return response;
}

// Check if the client sent a valid GET request...
int valid_request(char** rq, int num_tokens){
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

int set_mime_type(char* filepath, struct http_response* rsp){
    const char *ext;
    int filepath_len = strlen(filepath);
    
    // Reference: https://stackoverflow.com/questions/5297248/how-to-compare-last-n-characters-of-a-string-to-another-string-in-c
    if (filepath_len >= 2){
        ext = &filepath[filepath_len-2];
        if(strncmp(ext, "js", 2) == 0){ rsp->header->content_type = "text/javascript"; return 1; }
    }
    
    if (filepath_len >= 3){
        ext = &filepath[filepath_len-3];
        if(strncmp(ext, "txt", 3) == 0){ rsp->header->content_type = "text/plain"; return 1; }
        if(strncmp(ext, "css", 3) == 0) { rsp->header->content_type = "text/css"; return 1;  }
        if(strncmp(ext, "jpg", 3) == 0){ rsp->header->content_type = "image/jpg"; return 1; }
    }

    if (filepath_len >= 4){
        ext = &filepath[filepath_len-4];
        if(strncmp(ext, "html", 4) == 0){ rsp->header->content_type = "text/html"; return 1; }
        if(strncmp(ext, "jpeg", 4) == 0){ rsp->header->content_type = "image/jpeg"; return 1; }
    } 
    
    rsp->header->content_type = "text/plain";
    return 1;
}

// As stated in Piazza (https://piazza.com/class/kxhl6o1cccn2oo?cid=61), 
// we are using the date in the format of <day-name>, <day> <month> <year> as the etag
int match_etag(char* header_info, struct stat st){
    char file_mtime[45] = {0};
    struct tm buff;
    struct tm* ptime = gmtime_r(&(st.st_mtime), &buff);
    strftime(file_mtime, sizeof(file_mtime), "%a, %d %b %Y", ptime);
        
    if(strncmp(file_mtime, header_info, strlen(file_mtime)) == 0){ return 1; }
    return 0;
}

// Gets the response status by handling conditional requests
int get_response_status(char** hreqs, int num_hr, struct stat st, struct http_response* rsp){
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

void parse_request_modify_response(char** request, int num_request_lines, struct http_response* response){

    // Check if there are request lines
    if(!request){ response->header->status = 400; return; }

    // Check if the GET request the client sent is valid...
    int num_words;
    char** get_request_line = split_string_char(request[0], ' ', -1, &num_words);
    if(!get_request_line || !valid_request(get_request_line, num_words)){ response->header->status = 400; return; }

    // Get the HTTP version and connection type from the request--send same version in response
    response->header->version = get_request_line[2];
    if (strncmp(response->header->version, "HTTP/1.0", strlen(response->header->version)) == 0) {
        response->header->connection = "Connection: close"; 
    }
    if (strncmp(response->header->version, "HTTP/1.1", strlen(response->header->version)) == 0) {
        response->header->connection = "Connection: keep-alive"; 
    }
    for (int i=1; i<num_request_lines; i++) {
        if (strncmp("Connection: keep-alive", request[i], strlen("Connection: keep-alive")) == 0){
            response->header->connection = "Connection: keep-alive"; 
        }

        if (strncmp("Connection: close", request[i], strlen("Connection: close")) == 0){
            response->header->connection = "Connection: close"; 
        }
    }

    // Get file client requested--starting from root directory...
    char* filepath = malloc(sizeof(char) * (strlen(http_root_path) + strlen(get_request_line[1]) + 2));
    strcpy(filepath, http_root_path);
    strncat(filepath, get_request_line[1], strlen(get_request_line[1]));
    
    printf("Client requested: %s\n", filepath);

    if(access(filepath, F_OK) == 0){
        struct stat st; stat(filepath, &st);
        if (!S_ISREG(st.st_mode)){ response->header->status = 400; }
        else if(!set_mime_type(filepath, response)){ response->header->status = 501; }
        else {
            stat(filepath, &st);
            response->header->status = get_response_status(request, num_request_lines, st, response);
            response->header->content_length = st.st_size;
            response->body->fp = fopen(filepath, "r");
        }
        
    } else { response->header->status = 400; }

    free(filepath);
}

// Returns the http response header as a str
char* response_header_to_str(struct http_header* header){
    char* header_rsp = malloc(sizeof(char) * 1024);
    char status_msg[128];

    if(header->status == 200){
        sprintf(header_rsp,
                "%s 200 OK\r\n"
                "Content-Type: %s\r\n"
                "%s\r\n"
                "Content-Length: %d\r\n\r\n",
                header->version, header->content_type, header->connection, (int) header->content_length);

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
