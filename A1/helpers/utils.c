#include "*.h"

void error_exit(char* message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

int min(int a, int b){
    if(a >= b){
        return b;
    }
    return a; 
}


// References: https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
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

char* strip_leading_whitespace(char* str){
    while(isspace((unsigned char) *str)){
        str++;
    }
    return str;
}

// References: https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
// Splits the header by character delimiters
char** split_string_char(char* str, char delim, int stop_point, int* num_tokens){
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
    if(stop_point > 0){
        count = min(stop_point, count);
    }
    count += last_delim < (str + strlen(str) - 1);

    tokens = malloc(sizeof(char*) * count);
    char* tmpstr = str;
    if(tokens){
        for(int i = 0; i < count; i++){
            if(i == count - 1){
                tokens[i] = strip_leading_whitespace(tmpstr);
            }else{
                token = strtok_r(tmpstr, &delim, &tmpstr);
                if(token){
                    tokens[i] = token;
                }
            } 
        }
    }
    
    *num_tokens = count; 
    return tokens;
}

time_t get_time(char* str, const char* format){
    struct tm parsed_time;
    strptime(str, format, &parsed_time);
    time_t lt = mktime(&parsed_time);
    return lt;
}

int get_port(char const *arg){
    // Check if we got a valid port #
    for (int i=0; i < strlen(arg); i++){
        if (!isdigit(arg[i])){
            error_exit("port # is invalid...");
        }
    }

    // All good!
    return atoi(arg);
}