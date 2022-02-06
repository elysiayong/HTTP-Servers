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
    int server_fd = setup_server(port_number, http_root_path, 5);

    int client_fd = accept(server_fd, (struct sockaddr *) NULL, NULL);
    
    // 2. Start server, then wait and respond to clients...
    for(;;){
        if (!handle_client(client_fd, server_fd)){
            close(client_fd);
            client_fd = accept(server_fd, (struct sockaddr *) NULL, NULL);
        }
    }

    // 3. Shutdown server!
    close(server_fd);

    return 0;
}
