#define MAXLINE 2048
#define MAXBUFFER 50000 // max amount of bytes a client can send thru pipelining

int setup_server(int, char const *, int, int);
int handle_client(int);
int handle_client_pipeline(int); 
