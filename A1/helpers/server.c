#include "*.h"

// References: 
// - https://www.youtube.com/watch?v=esXw4bdaZkc, 
// - https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
// - https://beej.us/guide/bgnet/html/index-wide.html

int setup_server(int port, char const *http_root_path, int back_log_capacity) {

    // Allocates a new TCP socket
    int server_fd;
    if( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ error_exit("Could not allocate socket for server...\n"); }
    
    // Setup address to listen on
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // responding to anything
    server_address.sin_port = htons(port);

    // Bind server's socket to that address
    if((bind(server_fd, (const struct sockaddr *)&server_address, sizeof(server_address))) != 0){ error_exit("Bind error...\n"); }

    // Let server listen from the serverfd file descriptor, then wait for clients...
    if((listen(server_fd, back_log_capacity)) < 0){ error_exit("Listening error...\n"); }

    printf("Now listening on PORT %d!\n", port);
    
    fflush(stdout);

    return server_fd;
}

int handle_client(int client_fd, int server_fd) {
    
    // printf("Client sending request to server at fd: %d!\n", client_fd);
    
    char raw_request[MAXLINE+1] = {0}; int n = 0;

    // 1. Read client message
    n = read(client_fd, raw_request, MAXLINE-1);

    // TODO: HANDLE BAD REQUESTS if: bad_request(request): print error

    if(n > 0) {
        // Replace crlf+crlf with array terminator
        raw_request[n-4] = '\0';

        // Separate the request--line by line, into an array of strings for easy-read
        int num_request_lines = 0;
        char** request = split_string(raw_request, "\r\n", &num_request_lines); 

        // Initialize the response
        struct http_response* response = init_response();
        // TODO: CHECK IF EVERYTHING WAS MALLOC'D

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
            if(fread(buffer + header_size, 1, response->header->content_length, response->body->fp) != response->header->content_length){
                fclose(response->body->fp);
            }
            fclose(response->body->fp);
        }

        write(client_fd, buffer, buffer_size - 1); 

        // TODO: FREE!

        return 1;

    } else {
        return 0;
    }
}
