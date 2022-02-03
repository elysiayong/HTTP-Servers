#include "helpers/*.h"

// ----------------------------- Server ---------------------------------

int port_number;
const char* http_root_path;

/*** 
 * SIMPLESERVER!
 */
int main(int argc, char const *argv[])
{

    // Check if we have enough arguments...
    if (argc < 3) { 
        error_exit("port # and/or http root path were not given...");
    }

    port_number = get_port(argc, argv);
    http_root_path = argv[2];

    // 1. Create socket + bind on an address and listen...
    int server_fd = setup_server(port_number, http_root_path, 5);
    
    // 2. Start server, then wait and respond to clients...
    for(;;){

        int client_fd = accept(server_fd, (SA *) NULL, NULL); // client found

        handle_client(client_fd, server_fd);

        close(client_fd);

    }

    // 3. Shutdown server! TODO: Free memory too
    close(server_fd);

    return 0;
}
