#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message);

int main(int argc, char** argv) 
{
    int sock;
    struct sockaddr_in recv_adr;

    if(argc!=3){
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);//fd
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    recv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1)
        error_handling("connect() error");
    else
        puts("Connecting.........");
    
    write(sock, "123", sizeof("123"));
    send(sock, "45", sizeof("45"), MSG_OOB);//紧急
    write(sock, "567", sizeof("567"));
    send(sock, "890", sizeof("890"), MSG_OOB);//紧急


    close(sock);

    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}