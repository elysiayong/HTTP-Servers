#include "*.h"

// References: 
// - https://www.youtube.com/watch?v=esXw4bdaZkc, 
// - https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
// - https://beej.us/guide/bgnet/html/index-wide.html

int setup_server(int port, char const *http_root_path, int back_log_capacity) {

    // Allocates a new tcp socket
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){ error_exit("Could not allocate socket for server.\n"); }
    
    // Setup address to listen on
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // responding to anything
    serveraddr.sin_port = htons(port);

    // Bind server's socket to the address
    if((bind(server_fd, (SA *)&serveraddr, sizeof(serveraddr))) != 0){ error_exit("Bind error"); }

    // Let server listen from the serverfd file descriptor
    if((listen(server_fd, back_log_capacity)) < 0){ error_exit("Listen error"); }

    printf("Now listening on PORT %d!\n", port);
    fflush(stdout);

    return server_fd;
}

// Writes the http response into a buffer and sends it to the client
void answer_client(int client_fd, struct http_response* resp){ 
    // TODO: Send body IFF resp->header->status is 200  
    
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

    char* header = generate_header(resp->header);
    size_t header_size = strlen(header);

    int buffer_size = header_size + 1;

    // Only add body to buffer if status is 200
    if(resp->header->status == 200){
        buffer_size += resp->header->content_length;
    }

    char *buffer = malloc(sizeof(char) * buffer_size);
    sprintf(buffer, header, header_size);

    if(resp->header->status == 200){
        if(fread(buffer + header_size, 1, resp->header->content_length, resp->body->fp) != resp->header->content_length){
            fclose(resp->body->fp);
            error_exit("Uh oh...Skinky!");
        };
        fclose(resp->body->fp);
    }
    

    write(client_fd, buffer, buffer_size - 1);
    free(resp);
    free(buffer);
    free(header);
}

void handle_client(int client_fd, int server_fd) {

    printf("Client connected to server: %d!\n", client_fd);
    
    char recvline[MAXLINE+1] = {0};
    int n = 0;

    memset(recvline, 0, MAXLINE);

    // Read client message
    while((n = read(client_fd, recvline, MAXLINE-1)) > 0){
        if(strstr((char*) recvline, "\r\n\r\n")){
            break;
        }
        memset(recvline, 0, MAXLINE);
    }

    if(n < 0){
        close(client_fd); 
        error_exit("read error");
    }

    // Replace crlf+crlf with array terminator
    recvline[n-4] = '\0';
    struct http_response* response = type_response(recvline);

    if(response == NULL){
        close(client_fd);
        error_exit("Could not get response from client.");
    }

    answer_client(client_fd, response);
}
