#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

void error_exit(char*);
char** split_string(char*, char*, int*);
char** split_string_char(char*, char, int, int*);
time_t get_time(char*, const char*);

int get_port(char const *arg);