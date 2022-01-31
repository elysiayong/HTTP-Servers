#include "server.h"

// NOTE: This server still needs to be polished and requires further testing
// *NOTE: Put the helpers into separate .c file
// **NOTE: Update Makefile to compile .c with .h
int port;
char http_path[MAXLINE] = "";
char http_header_terminator[5] = "\r\n\r\n";

struct http_response{
    struct http_header* header;
    struct http_body* body;
};

struct http_header{
    char* content_type;
    int status;
    size_t content_length;
};

struct http_body{
    FILE* fp;
};

// --------------------------------- Utilities --------------------------------------------

// Reserved for server side errors only
// Probably wanna update to give more informative messages or something 
void error_exit(char* message){
    printf("%s\n", message);
    exit(1);
}

// NOTE: There are two split strings bc C is terrible
// One splits the header by /r/n; the other splits by character delimiters
// If you can find a better way/refactor please do, I die a little seeing repeated code and im tired of c
char** split_string(char* str, char* delim, int* num_tokens){
    char** tokens = 0;
    char* token;
    char* tmp = str;
    int count = 0;
    char* last_delim = 0;

    while(*tmp){
        if(strncmp(tmp, delim, strlen(delim)) == 0){
            count++;
            last_delim = tmp;
        }
        tmp++;
    }
    count += last_delim < (str + strlen(str) - 1);

    tokens = malloc(sizeof(char*) * count);
    char* tmpstr = str;
    if(tokens){
        for(int i = 0; i < count; i++){
            token = strtok_r(tmpstr, delim, &tmpstr);
            if(token){
                tokens[i] = token;
            }
        }
    }
    
    *num_tokens = count; 
    return tokens;
}

char** split_string_char(char* str, char delim, int* num_tokens){
    char** tokens = 0;
    char* token;
    char* tmp = str;
    int count = 0;
    char* last_delim = 0;

    while(*tmp){
        if(*tmp == delim){
            count++;
            last_delim = tmp;
        }
        tmp++;
    }
    count += last_delim < (str + strlen(str) - 1);

    tokens = malloc(sizeof(char*) * count);
    char* tmpstr = str;
    if(tokens){
        for(int i = 0; i < count; i++){
            token = strtok_r(tmpstr, &delim, &tmpstr);
            if(token){
                tokens[i] = token;
            }
        }
    }
    
    *num_tokens = count; 
    return tokens;
}

struct http_response* init_response(){
    struct http_response *response = malloc(sizeof(struct http_response*)); 
    if(response == NULL) return NULL;

    response->header = malloc(sizeof(struct http_header*));
    if(response == NULL) return NULL;

    response->body = malloc(sizeof(struct http_body*));
    if(response == NULL) return NULL;

    return response;
}

// ------------------------------------- HTTP helpers ----------------------------------------

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
        char* info = header_info[1];

        // Check for conditionals, set rsp and return accordingly
        if(strncmp(header, "Last-Modified", strlen("Last-Modified")) == 0){

        }if(strncmp(header, "ETag", strlen("ETag")) == 0){
            
        }if(strncmp(header, "If-Match", strlen("If-Match")) == 0){
            
        }if(strncmp(header, "If-None-Match", strlen("If-None-Match")) == 0){
            
        }if(strncmp(header, "If-Modified-Since", strlen("If-Modified-Since")) == 0){
            
        }if(strncmp(header, "If-UnModified-Since", strlen("If-UnModified-Since")) == 0){
            
        }
    }
}

// Returns the http response header depending on the status 
char* generate_header(int connfd, struct http_header* header){
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


// ------------------------------- Request + Response ----------------------------------------

// Handles request appropriately and generates an http response
// NOTE: Messy looking becauase of error checks will need to polish
struct http_response* handle_request(char* line){
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
    char* filepath = malloc(sizeof(char) * strlen(request_line[1] + 1));
    strcat(filepath, ".");
    strcat(filepath, request_line[1]);

    // Checks if the file exists
    if(access(filepath, F_OK) == 0){
        if(!set_mime_type(filepath, response)){
            response->header->status = 400;
        };

        struct stat st;
        stat(filepath, &st);
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

// Writes the http response into a buffer and sends it to the client
void handle_response(int connfd, struct http_response* resp){ 
    // TODO: Send body IFF resp->header->status is 200  
    
    // NOTE: There's probably a much better way to write this
    // Currently what it does:
    // 1) Generate an http header
    // 2) Create a buffer with the size of header + body + null terminator
    // 3) Write the http header into buffer
    // 4) Read file data into buffer 
    // 5) Write buffer to client socket
    // 
    // And no, you cannot write the header to socket first, the write body to socket
    // I tried that, the client doesn't treat the delayed payload as a singular response, generating garbage.
    // 
    // ALSO this section needs error checking

    char* header = generate_header(connfd, resp->header);
    size_t header_size = strlen(header);
    int buffer_size = header_size + resp->header->content_length + 1;
    char *buffer = malloc(sizeof(char) * buffer_size);
    
    sprintf(buffer, header, header_size);

    if(fread(buffer + header_size, 1, resp->header->content_length, resp->body->fp) != resp->header->content_length){
        fclose(resp->body->fp);
        error_exit("Uh oh...Skinky!");
    };

    write(connfd, buffer, buffer_size - 1);

    close(connfd);
    fclose(resp->body->fp);
    free(resp);

}

// ----------------------------- Server ---------------------------------

// The code in setup() is referenced from ...[yt link here] i'll put the link later i forgot where i saved it 
int setup(){
    int listenfd, connfd, n;
    struct sockaddr_in serveraddr;
    char recvline[MAXLINE+1] = {0};

    // Allocates a new tcp socket
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        error_exit("Socket error\n");
    }
    
    // Setup address to listen on
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // responding to anything
    serveraddr.sin_port = htons(PORT);

    // Bind listening socket to the address
    if((bind(listenfd, (SA *) &serveraddr, sizeof(serveraddr))) < 0){
        error_exit("Bind error");
    }
    if((listen(listenfd, 10)) < 0){
        error_exit("Listen error");
    }

    // Start server
    for(;;){
        printf("Waiting for a connection on PORT %d\n", PORT);
        fflush(stdout);

        // Socket to send data to the client
        connfd = accept(listenfd, (SA *) NULL, NULL);
        memset(recvline, 0, MAXLINE);

        // Read client message
        while((n = read(connfd, recvline, MAXLINE-1)) > 0){
            if(strstr((char*) recvline, http_header_terminator)){
                break;
            }
            memset(recvline, 0, MAXLINE);
        }

        if(n < 0){
            close(connfd); 
            error_exit("read error");
        }

        // Replace crlf+crlf with array terminator (move this into handle_request if it's cleaner)
        recvline[n-4] = '\0';
        struct http_response* response = handle_request(recvline);

        if(response == NULL){
            close(connfd);
            error_exit("Uh oh, skinky!");
        }

        handle_response(connfd,response);

        // TODO: Make sure to free stuff
        
    }

    return 0;
}


int main(int argc, char const *argv[])
{
    // TODO: Add stdin constraints n stufffffffffff, replace port and http_path
    //       http_path is the server home directory

    // fscanf(stdin, "%d", &port);

    // printf("Enter http root path: ");
    // fscanf(stdin, "%s",http_path);

    setup();
    return 0;
}
