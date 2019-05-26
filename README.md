# 从0实现一个HTTP Web Server，记录过程


# 1、阅读《TCP网络编程》
## 接口大全

#### int socket(int domain, int type, int protocol)    
return: fd | -1    params: 1、PF_INET | PF_INET6 | etc. 2、SOCK_STREAM (tcp) | SOCK_DGRAM (udp)  3、IPPROTO_TCP | IPPROTO_UDP
#### int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen)    
return: 1 | 0    params: 1、fd  2、结构体（地址族和地址信息） 3、地址长度
#### int listen(int sockfd, int backlog)    
return: 0 | -1    params: 1、希望进入等待连接请求状态的套接字文件描述符fd  2、连接请求等待队列的长度
#### int accept(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
return: fd | -1    params: 1、fd 2、保存客户端地址信息的结构体（地址族和地址信息） 3、地址长度
#### int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
return: 0 | -1    params: 1、fd 2、保存服务器地址信息的结构体（地址族和地址信息） 3、地址长度

#### unsigned short/long htons/htonl(unsigned short/long)   
字节序转换 host to network  #include <arpa/inet.h>  
return: 网络字节   params: 主机字节
#### unsigned short/long ntohs/ntohl(unsigned short/long)   
字节序转换 network to host  #include <arpa/inet.h>  
return: 主机字节   params: 网络字节

#### int inet_aton(const char *string, struct in_addr* addr)    
网络地址转换:127.232.124.79(char* | str) -> 0x4f7ce87f(int)  
return: 1 | 0    params: 1、字符串 2、结构体in_addr，内部是32位的IPv4地址 -> in_addr_t(POSIX标准下的uint32_t)
#### char* inet_ntoa(struct in_addr adr)
网络字节序整数型IP地址 -> str  
return: 指向字符串的指针    params: 32位的IPv4地址


