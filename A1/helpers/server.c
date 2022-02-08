#include "*.h"

// References: 
// - https://www.youtube.com/watch?v=esXw4bdaZkc, 
// - https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
// - https://beej.us/guide/bgnet/html/index-wide.html

int setup_server(int port, char const *http_root_path, int back_log_capacity) {

    // Allocates a new TCP socket
    int server_fd, opt;
    if( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ error_exit("Could not allocate socket for server...\n"); }
    
    // Setup address to listen on
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // responding to anything
    server_address.sin_port = htons(port);

    //
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, (socklen_t)sizeof(int)) != 0){
        error_exit("Setsockopt error...\n");
    }

    // Bind server's socket to that address
    if((bind(server_fd, (const struct sockaddr *)&server_address, sizeof(server_address))) != 0){ 
        error_exit("Bind error...\n"); 
    }

    // Let server listen from the serverfd file descriptor, then wait for clients...
    if((listen(server_fd, back_log_capacity)) < 0){ 
        error_exit("Listening error...\n"); 
    }

    printf("Now listening on PORT %d!\n", port);
    
    fflush(stdout);

    return server_fd;
}

int process_client_request(char* raw_request, int client_fd){
    // Separate the request--line by line, into an array of strings for easy-read
    int num_request_lines = 0;
    char** request = split_string(raw_request, "\r\n", &num_request_lines); 

    // Initialize the response
    struct http_response* response = init_response();
    
    // Check that we were able to malloc everything
    if (!response || 
        !response->header || !response->header->content_type || !response->header->version || !response->header->connection ||
        !response->body || !response->body->fp) 
        {
            error_exit("Could not malloc response for client's request...\n"); 
        }

    // 2. Parse request, modify response accordingly
    parse_request_modify_response(request, num_request_lines, response); 

    // 3. Send response to client
    char* header = response_header_to_str(response->header);
    size_t header_size = strlen(header);

    int buffer_size = header_size + 1;

    // Only add body to buffer if status is 200
    if(response->header->status == 200){ buffer_size += response->header->content_length; }

    char *buffer = malloc(sizeof(char) * buffer_size);
    sprintf(buffer, header, header_size);

    if(response->header->status == 200){
        fread(buffer + header_size, 1, response->header->content_length, response->body->fp);
        fclose(response->body->fp);
    }else{
        free(response->body->fp);
    }

    write(client_fd, buffer, buffer_size - 1); 


    free(buffer);
    free(header);

    // free(response->header->content_type); 
    // free(response->header->version); 
    // free(response->header->connection); 
    free(response->header);
    free(response->body);
    free(response);
    free(request);

    return 1;
}


int handle_client_pipeline(int client_fd){
    char raw_pipelined_requests[MAXLINE+1] = {0};

    if(read(client_fd, raw_pipelined_requests, MAXLINE-1) > 0){
        int keep_alive = 0;
        char *request_tail, *request, *request_head;

        request_head = raw_pipelined_requests;
        
        while((request_tail = strstr(request_head, "\r\n\r\n")) != NULL) {
            // Gets a request from the pipelined requests
            int request_len = request_tail - request_head + 1;
            request = (char *)malloc(request_len * sizeof(char));
            memset(request, '\0', request_len);
            strncpy(request, request_head, request_len);
            
            // Process the request
            keep_alive = process_client_request(request, client_fd);
            free(request);

            // Move to next pipelined request
            request_head = request_tail + strlen("\r\n\r\n");
        }
        return keep_alive;
    }else{
        return 0;
    }
}

int handle_client(int client_fd) {
    char raw_request[MAXLINE+1] = {0}; 
    int n = read(client_fd, raw_request, MAXLINE-1);

    if(n > 0) {
        // Replace crlf+crlf with array terminator
        raw_request[n-4] = '\0';
        return process_client_request(raw_request, client_fd);
    }
    return 0;
}
