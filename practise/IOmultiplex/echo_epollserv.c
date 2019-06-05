#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#define BUF_SIZE 4
#define EPOLL_SIZE 50

void error_handling(char* message);
void setnonblockingmode(int fd);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock; //服务器和客户端的socketfd
    struct sockaddr_in serv_adr, clnt_adr; //服务器和客户端的地址
    socklen_t adr_sz; //address size

    int str_len, i;
    char buf[BUF_SIZE];

    struct epoll_event* ep_events;//保存发生事件的文件描述符集合的结构体地址
    struct epoll_event event;
    int epfd, event_cnt;

    if(argc!=2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    //服务器socketfd
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    //bind and listen
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    //创建保存epoll文件描述符的空间，epoll创建的文件描述符保存空间称为epoll例程
    epfd = epoll_create(EPOLL_SIZE);
    //分配一个足够大的空间来存放发生事件的文件描述符集合
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    event.events = EPOLLIN; //需要读数据时
    event.data.fd = serv_sock; 
    //添加服务器socketfd到epoll例程epfd中，监视事件为需要读数据时（第四个参数
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1)
    {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//等待发生变化，有变化返回变化的文件描述符数
        if(event_cnt == -1)
        {
            puts("epoll_wait() error");
            break;
        }
        puts("return epoll_wait! " );

        for(int i = 0; i<event_cnt; ++i)
        {
            if(ep_events[i].data.fd == serv_sock)//有连接请求
            {
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                setnonblockingmode(clnt_sock);
                event.events = EPOLLIN | EPOLLET;//EPOLLET为边缘触发方式
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//添加连接的客户端socketfd，监视读数据事件
                printf("connected client: %d \n", clnt_sock);    
            }
            else//read message 有待读取的数据
            {
                while(1)//边缘触发方式下需要循环调用read来读取输入缓冲中的所有数据
                {
                    str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if(str_len == 0)//close request 关闭连接
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL); //从epfd中清除掉这个fd
                        close(ep_events[i].data.fd); //关闭fd
                        printf("closed client: %d \n", ep_events[i].data.fd);
                        break;
                    }
                    else if(str_len<0){
                        if(errno == EAGAIN)
                            break;
                    }
                    else
                    {
                        write(ep_events[i].data.fd, buf, str_len);//echo! 把收到的数据发送回去
                    }
                }
                
            }
            
        }
    }
    close(serv_sock);
    close(epfd);

    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFD, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}