# 从0实现一个HTTP Web Server，记录过程


# 1、阅读《TCP网络编程》
## 接口大全

#### int socket(int domain, int type, int protocol)    
#####return: fd | -1    parameters: 1、PF_INET  2、SOCK_STREAM (tcp) | SOCK_DGRAM (udp)  3、IPPROTO_TCP | IPPROTO_UDP
#### int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen)    
#####return: 1 | 0    parameters: 1、
#### int listen(int sockfd, int backlog)    
#####return: 0 | -1
#### int accept(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
#####return: fd | -1
#### int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
#####return: 0 | -1
#### unsigned short/long htons/htonl(unsigned short/long)   
#####字节序转换 host to network  include <arpa/inet.h>
#### unsigned short/long ntohs/ntohl(unsigned short/long)   
#####字节序转换 network to host  include <arpa/inet.h>
#### int inet_aton(const char *string, struct in_addr* addr)    
#####return: 1 | 0    网络地址转换:127.232.124.79(char* | str) -> 0x4f7ce87f(int)
