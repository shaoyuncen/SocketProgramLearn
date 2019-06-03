#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc!=2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);  //注册服务器端套接字，加入监视对象，服务器端有接收的数据，意味着有新的连接请求
    fd_max = serv_sock;//设置监视范围

    while(1)
    {
        cpy_reads = reads;//保存原有fd状态
        timeout.tv_sec = 5;//设置超时
        timeout.tv_usec = 5000;
        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)//调用select，只监视读入数据
            break;
        if(fd_num == 0)//没有fd发生变化
            continue;
        
        //有fd发生变化->有读入数据(发生请求时)
        for(int i = 0; i<fd_max+1; ++i)
        {
            if(FD_ISSET(i, &cpy_reads))//轮询fd_set查找发生变化的fd
            {
                if(i == serv_sock)//connection request 发生连接请求
                {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads); //将客户端socket注册进fd_set
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected cliend: %d \n", clnt_sock);
                }
                else//read message 有待读取的数据
                {
                    str_len = read(i, buf, BUF_SIZE);
                    if(str_len == 0)//close request 关闭连接
                    {
                        FD_CLR(i, &reads); //从fd_set中清除掉这个fd
                        close(i); //关闭fd
                        printf("closed client: %d \n", i);
                    }
                    else
                    {
                        write(i, buf, str_len);//echo! 把收到的数据发送回去
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}