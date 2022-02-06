#include "helpers/*.h"

int port_number;
const char* http_root_path;

/*** 
 * SIMPLESERVER!
 */
int main(int argc, char const *argv[])
{
    // Check if the server was given enough arguments...
    if (argc < 3) { error_exit("port # and/or http root path were not given..."); }

    port_number = get_port(argv[1]);
    http_root_path = argv[2];

    // 1. Create socket + bind on an address, then listen...
    int server_fd = setup_server(port_number, http_root_path, 5);
    
    // 2. Start server, then wait and respond to clients...
    for(;;){
        
        int client_fd;
        if ( (client_fd = accept(server_fd, (struct sockaddr *) NULL, NULL)) < 0){ 
            printf("Could not accept client to server...\n"); 
            
        } else {

            handle_client(client_fd, server_fd);

            printf("Closing client connection at fd: %d\n", client_fd);
            close(client_fd); // non-persistent...
        }
    }

    // 3. Shutdown server!
    close(server_fd);

    return 0;
}
