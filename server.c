#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


void write_only(int fd){
    char buffer[10000];
    do{
        memset(buffer, '\0', sizeof(buffer));
        scanf("%s", buffer);
        write(fd, buffer, strlen(buffer));
    } while(strncmp(buffer, "bye", 3) != 0);
}


int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int err = 0;

    int enable = 1;
    setsockopt(
        fd, SOL_SOCKET, SO_REUSEADDR, (void*)&enable, sizeof(int)
    );

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5000);

    err = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(err != 0){
        printf("Error when binding socket\n");
        return 1;
    }

    err = listen(fd, 1);
    if(err != 0){
        printf("Error when listening socket\n");
        return 1;
    }

    struct sockaddr_in cli;
    int len = sizeof(cli);
    int cliFd = accept(fd, (struct sockaddr*)&cli, &len);
    if(cliFd <= 0){
        printf("Error in accepting socket\n");
        return 1;
    }

    write_only(cliFd);

    shutdown(cliFd, 2);
    close(cliFd);

    shutdown(fd, 2);
    close(fd);

    return 0;
}
