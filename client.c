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

    char addrHost[1000];
    printf("Enter host: ");
    scanf("%s", addrHost);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(addrHost);
    addr.sin_port = htons(5000);

    connect(fd, (struct sockaddr*)&addr, sizeof(addr));

    write_only(fd);
    shutdown(fd, 2);
    close(fd);

    return 0;
}
