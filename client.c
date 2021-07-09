#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


char buffer[1000];


void rmFirstNChar(char* string, int n){
    int len = strlen(string);
    int idx = 0;
    for(int i=n; i<len; i++){
        string[idx++] = string[i];
    }
    for(int i=idx; i< idx+n; i++){
        string[i] = '\0';
    }
}


void exec_data(char* string){
    int pid = fork();
    if(pid != 0) return;

    // tokenize string
    char* tokenBuffer[1000];
    int len = strlen(string);
    int idx = 0;
    int prevIsSpace = 1;
    int stringMode = 0;
    for(int i=0; i<len; i++){
        // enable string mode
        if(stringMode == 0 && string[i] == '"') stringMode++;
        else if(stringMode == 1 && string[i] == '"') stringMode++;
        else if(stringMode == 1) stringMode--;

        // disable string mode
        else if(stringMode == 3 && string[i] == '"') stringMode++;
        else if(stringMode == 4 && string[i] == '"') stringMode++;
        else if(stringMode == 4) stringMode--;

        // parse string
        if(stringMode < 2){
            if(string[i] != '_' && prevIsSpace == 1){
                tokenBuffer[idx++] = &string[i];
                prevIsSpace = 0;
            }
            else if(string[i] == '_'){
                string[i] = '\0';
                prevIsSpace = 1;
            }
        }
        else{
            // clean mode detection
            if(stringMode == 2){
                rmFirstNChar(&string[i-1], 2);
                stringMode++;
            }
            if(stringMode == 5){
                rmFirstNChar(&string[i-1], 2);
                stringMode = 0;
            }
            if(string[i] == '_') string[i] = ' ';
        }
        if(string[i] == '\n') string[i] = '\0';
    }
    tokenBuffer[idx] = NULL;

    // exec shell
    idx = 0;
    printf("shell token: ");
    while(tokenBuffer[idx] != NULL){
        printf("%s ", tokenBuffer[idx++]);
    }
    printf("\n");
    execvp(tokenBuffer[0], tokenBuffer);
}


void read_only(int fd){
    do{
        memset(buffer, '\0', 1000);
        int textLen = read(fd, buffer, 1000);
        if(textLen <= 0) break;

        // check newline
        if(buffer[strlen(buffer)-1] != '\n'){
                int len = strlen(buffer);
                buffer[len] = '\n';
                buffer[len+1] = '\0';
        }
        printf("server send: %s", buffer);
        exec_data(buffer);
    } while(strncmp(buffer, "bye", 3) != 0);
}


int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(5000);

    connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    read_only(fd);
    shutdown(fd, 2);
    close(fd);
    return 1;
}
