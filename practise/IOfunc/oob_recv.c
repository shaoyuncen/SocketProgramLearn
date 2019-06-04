#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 30

void error_handling(char* message);
void urg_handler(int signo);

int acpt_sock;
int recv_sock;

int main(int argc, char** argv) 
{
    struct sockaddr_in serv_adr, recv_adr;
    int str_len, state;
    socklen_t serv_adr_sz;

    struct sigaction act;
    char buf[BUF_SIZE];


    if(argc!=2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = urg_handler;//注册回调函数
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(acpt_sock == -1)//监听的socket
        error_handling("socket() error");
    
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(acpt_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1)
        error_handling("bind() error");

    if(listen(acpt_sock, 5) == -1)
        error_handling("listen() error");
    
    serv_adr_sz = sizeof(serv_adr);
    
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);//接收数据的socketfd
    //文件描述符recv_sock指向的套接字 引发的SIGURG信号处理进程 变为 将getpid()函数返回值用作ID的进程
    fcntl(recv_sock, F_SETOWN, getpid()); //把fd recv_sock的拥有者改为getpid()函数返回的进程
    state = sigaction(SIGURG, &act, 0); //有紧急信号则调用act中的handler

    while((str_len=recv(recv_sock, buf, sizeof(buf), 0))!=0)
    {
        if(str_len == -1)//没收到数据
            continue;
        buf[str_len] = 0;//给接收的字符后面添加一个\0
        puts(buf);
    }


    close(recv_sock);
    close(acpt_sock);

    return 0;
}
void urg_handler(int signo)
{
    int str_len;
    char buf[BUF_SIZE];
    str_len = recv(recv_sock, buf, sizeof(buf)-1, MSG_OOB);
    buf[str_len] = 0;
    printf("Urgent message: %s \n", buf);
}
void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}