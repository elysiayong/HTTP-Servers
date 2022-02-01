#define MAXLINE 4096
#define SA struct sockaddr

int setup_server(int, char const *, int);
void handle_client(int, int);