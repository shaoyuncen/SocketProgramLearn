# 从0实现一个HTTP Web Server，记录过程


# 1、阅读《TCP网络编程》
## 接口大全

#### 网络相关
#### int socket(int domain, int type, int protocol)    
    return: fd | -1    
    params: ①PF_INET | PF_INET6 | etc. ②SOCK_STREAM (tcp) | SOCK_DGRAM (udp)  ③IPPROTO_TCP | IPPROTO_UDP
#### int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen)    
    return: 1 | 0    
    params: ①fd  ②结构体（地址族和地址信息） ③地址长度
#### int listen(int sockfd, int backlog)    
    return: 0 | -1    
    params: ①希望进入等待连接请求状态的套接字文件描述符fd  ②连接请求等待队列的长度
#### int accept(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
    return: fd | -1    
    params: ①fd ②保存客户端地址信息的结构体（地址族和地址信息） ③地址长度
#### int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen)    
    return: 0 | -1    
    params: ①fd ②保存服务器地址信息的结构体（地址族和地址信息） ③地址长度

#### unsigned short/long htons/htonl(unsigned short/long)   
    #字节序转换 host to network  #include <arpa/inet.h>  
    return: 网络字节   
    params: 主机字节
#### unsigned short/long ntohs/ntohl(unsigned short/long)   
    #字节序转换 network to host  #include <arpa/inet.h>  
    return: 主机字节   
    params: 网络字节

#### in_addr_t inet_addr(const char* string)
    return: 32位大端序整数型值 | INADDR_NONE
    params: str
#### int inet_aton(const char *string, struct in_addr* addr)    
    #网络地址转换:127.232.124.79(char* | str) -> 0x4f7ce87f(int)  
    return: 1 | 0    
    params: ①字符串 ②结构体in_addr，内部是32位的IPv4地址 -> in_addr_t(POSIX标准下的uint32_t)
#### char* inet_ntoa(struct in_addr adr)
    #网络字节序整数型IPv4地址(struct in_addr) -> str  
    return: 指向字符串的指针    
    params: 32位的IPv4地址

#### ssize_t sendto(int sock, void* buff, size_t nbytes, int flags, struct sockaddr* to, socklen_t addrlen)  
    #UDP发送端函数  
    return: 成功时返回传输的字节数 | -1    
    params: ①UDP套接字fd ②保存待传输数据的缓冲地址值 ③待传输数据长度 ④可选项参数，没有则为0 ⑤目的地址 ⑥目的地址长度  
#### ssize_t recvfrom(int sock, void* buff, size_t nbytes, int flags, struct sockaddr* from, socklen_t addrlen)  
    #UDP接收端函数  
    return: 成功时返回传输的字节数 | -1    
    params: ①UDP套接字fd ②保存待传输数据的缓冲地址值 ③待传输数据长度 ④可选项参数，没有则为0 ⑤源地址 ⑥源地址长度     
#### UDP补充说明： UDP也可以使用connect来向套接字注册目标IP和端口号，然后使用sendto/recvfrom/read/write函数来传递数据。这并不意味着要与对方的UDP套接字连接，仅仅是注册了IP和port。  

#### int shutdown(int sock, int howto)  
    #优雅断开连接 -> 半关闭
    return: 0 | -1    
    params: ①需要断开的fd ②断开方式（SHUT_RD | SHUT_WR | SHUT_RDWR）-> (断开输入流|断开输出流|I/O均断开)  

#### struct hostent* gethostbyname(const char* hostname)   
    #通过域名获取IP地址  
    return: hostent结构体地址 | NULL指针    
    params: IP地址str
    #返回的结构体： 
    struct hostent{  
        ①char* h_name        #官方域名  
        ②char** h_aliases    #多个域名列表  
        ③int h_addrtype      #IP地址的地址族  
        ④int h_length        #IP地址长度  
        ⑤char** h_addr_list  #IP地址 
    };   

#### struct hostent* gethostbyaddr(const char* addr, socklen_t len, int family)
    return: hostent地址 | NULL
    params: ①in_addr结构的IP地址 ②IP地址长度(IPv4->4,IPv6->16) ③地址族信息(IPv4->AF_INET,IPv6->AF_INET6)

#### int getsockopt(int sock, int level, int optname, void* optval, socklen_t* optlen)
    # 查看套接字信息
    return: 0 | -1
    params: ①fd ②要查看的可选项的协议层 ③可选项名 ④保存查看结果的缓冲地址值 ⑤向第四个参数传递的缓冲大小
#### int setsockopt(int sock, int level, int optname, const void* optval, socklen_t optlen)
    # 设置套接字信息
    return: 0 | -1
    params: ①fd ②要更改的可选项的协议x可选项名 ④保存要更改的缓冲地址值 ⑤向第四个参数传递的缓冲大小
    第三个参数项部分参数: SO_REUSEADDR -> 将time_wait状态下的socket port重新分配给新的套接字
                       TCP_NODELAY ->  禁用Nagle算法(只有收到ACK后才继续传下一个数据包)默认是关闭的，传输大文件数据时可以考虑关闭

#### 进程相关
#### pid_t fork(void);  
    # 复制进程
    父进程return: 子进程ID | 失败返回-1
    子进程return: 0 | -1

#### pid_t wait(int* statloc)
    # 销毁僵尸进程，子进程终止时传递的返回值将保存到statloc所指内存空间，通过下列宏进行分离：
    # ①WIFEXITED正常终止时返回True ②WEXXITSTATUS返回子进程的返回值
    # 注意： 调用wait函数时如果没有已终止的子进程，那么程序将会进入阻塞直到有子进程终止
    return: 成功返回终止的子进程ID | -1
    params: 用于保存子进程终止时的返回值

#### pid_d waitpid(pid_t pid, int* statloc, int options)
    # 没有子进程也不阻塞的wait
    return: 成功时返回子进程的ID(或0) | -1
    params: ①等待终止的子进程ID,若为-1则与wait相同等待任意子进程 ②同wait的statloc ③传递头文件sys/wait.h中的常量WNOHANG，即使没有终止的子进程也不会阻塞，而是返回0并退出函数

#### void (*signal(int signo, void (*func)(int)))(int)
    # 为了在产生信号时调用，返回之前注册的函数指针  #include <signal.h>
    # 发生第一个参数的情况时，调用第二个参数所指的函数
    return: 参数类型为int型，返回void型函数指针
    params: ①第一个参数signo为特殊情况信息 ②第二个参数void (*func)(int)为特殊情况下要调用的函数的指针（回调函数） 
    signal函数中注册的部分特殊情况和对应的常数: SIGALRM -> 已到通过调用alarm函数注册的时间
                                          SIGINT -> 输入ctrl+C
                                          SIGCHLD -> 子进程终止
    示例：子进程终止则调用mychild函数 -> signal(SIGCHID, mychild)
         已到通过alarm函数注册的时间 -> signal(SIGALRM, timeout)
         输入CTRL+C时调用keycontrol函数 -> signal(SIGINT, keycontrol)

#### unsigned int alarm(unsigned int seconds)
    # 返回0或以秒为单位的距SIGALRM信号发生所剩时间，返回的SIGALRM信号将会唤醒处于阻塞态的进程，并调用Handler.
    return: unsigned int(秒) | 0
    params: 传递一个正整型参数，相应时间后将产生SIGALRM信号，若向该函数传递0，则取消之前对SIGALRM信号的预约
            若预约信号后未指定该信号对应的处理函数，则通过(调用signal函数)终止进程，不做任何处理

#### int sigaction(int signo, const struct sigaction* act, struct sigaction* oldact)
    # 可用于替换signal,并且更加常用
    return: 0 | -1
    params: ①与sigal函数相同 ②对应于第一个参数的信号处理器信息 ③通过此参数获取之前注册的信号处理函数指针，不需要则传递0
    struct sigaction{
        void (*sa_handler)(int);    # 信号处理函数的地址
        sigset_t sa_mask;           # 信号相关的选项和特性，均置为0即可
        int sa_flags;
    }

#### int pipe(int filedes[2])
    # 管道和套接字一样属于操作系统，所以fork时复制的仅是用于管道I/O的文件描述符
    return: 0 | -1
    params: filedes[0]->通过管道接收数据时使用的fd，即管道出口
            filedes[1]->通过管道发送数据时使用的fd，即管道入口

#### I/O复用技术
#### int select(int maxfd, fd_set* readset, fd_set* writeset,fd_set* exceptset, const struct timeval* timeout)