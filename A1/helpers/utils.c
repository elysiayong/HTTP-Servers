#include "*.h"

void error_exit(char* message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

// Splits the header by /r/n; 
char** split_string(char* str, char* delim, int* num_tokens){
    char** tokens = 0;
    char* token;
    char* tmp = str;
    int count = 0;
    char* last_delim = 0;

    while(*tmp){
        if(strncmp(tmp, delim, strlen(delim)) == 0){
            count++;
            last_delim = tmp;
        }
        tmp++;
    }
    count += last_delim < (str + strlen(str) - 1);

    tokens = malloc(sizeof(char*) * count);
    char* tmpstr = str;
    if(tokens){
        for(int i = 0; i < count; i++){
            token = strtok_r(tmpstr, delim, &tmpstr);
            if(token){
                tokens[i] = token;
            }
        }
    }
    
    *num_tokens = count; 
    return tokens;
}

// Splits the header by character delimiters
char** split_string_char(char* str, char delim, int* num_tokens){
    char** tokens = 0;
    char* token;
    char* tmp = str;
    int count = 0;
    char* last_delim = 0;

    while(*tmp){
        if(*tmp == delim){
            count++;
            last_delim = tmp;
        }
        tmp++;
    }
    count += last_delim < (str + strlen(str) - 1);

    tokens = malloc(sizeof(char*) * count);
    char* tmpstr = str;
    if(tokens){
        for(int i = 0; i < count; i++){
            token = strtok_r(tmpstr, &delim, &tmpstr);
            if(token){
                tokens[i] = token;
            }
        }
    }
    
    *num_tokens = count; 
    return tokens;
}

int get_port(int argc, char const *argv[]){
    
    // Check if we got a valid port #
    for (int i=0; i < strlen(argv[1]); i++){
        if (!isdigit(argv[1][i])){
            error_exit("port # is invalid...");
        }
    }

    // All good!
    return atoi(argv[1]);
}