#include "helpers/*.h"

// ----------------------------- Server ---------------------------------

/*** 
 * SIMPLESERVER!
 */
int main(int argc, char const *argv[])
{
    // Check if we have enough arguments...
    // if (argc < 3) { 
    //     error_exit("port # and/or http root path were not given...");
    // }
    
    // 1. Create socket + bind on an address and listen...
    int server_fd = setup_server(18000, "./test_websites/example/index.html", 5);

    // int server_fd = setup_server(get_port(argc, argv), argv[2], 5);
    
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
