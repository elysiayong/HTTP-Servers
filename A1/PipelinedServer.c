#include "helpers/*.h"

int port_number;
const char* http_root_path;

int main(int argc, char const *argv[])
{
    // Check if we have enough arguments...
    if (argc < 3) { error_exit("port # and/or http root path were not given..."); }

    port_number = get_port(argv[1]);
    http_root_path = argv[2];

    // 1. Create socket + bind on an address, then listen...
    int server_fd = setup_server(port_number, http_root_path, 20);

    struct timeval timeout;
    timeout.tv_usec = 0; timeout.tv_sec = 5;


    // 2. Start server, then wait and respond to clients...
    for(;;){
        int client_fd;
        if ( (client_fd = accept(server_fd, (struct sockaddr *) NULL, NULL)) < 0){ 
            printf("Could not accept client to server...\n"); 
            
        } else {
            int pid = fork();
            if(pid == -1){
               error_exit("Fork error"); 
            }

            if (pid == 0){
                setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

                while(handle_client_pipeline(client_fd));
                printf("Closed client connection at fd: %d...\n", client_fd);
                close(client_fd);
                break;
            }else{
                close(client_fd);
            }
        }
    } 

    // 3. Shutdown server!
    close(server_fd);

    return 0;
}
