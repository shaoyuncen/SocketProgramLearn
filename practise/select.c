#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    fd_set reads,temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    FD_ZERO(&reads); //初始化reads
    FD_SET(0, &reads); //注册标准输入fd(控制台)//0 is standard input(console)
    
    /*
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;
    */

    while(1)
    {
        temps = reads; //reads复制到temp，调用完select后未发生变化的fd会被置0，所以需要保存原始状态
        timeout.tv_sec = 5;//每次调用select前都要初始化timeval的时间
        timeout.tv_usec = 0;//因为调用select后timeval会被替换为超时前剩余时间
        result = select(1, &temps, 0, 0, &timeout);//如果有控制台输入则返回大于0的整数，否则超时返回0
        if(result == -1)
        {
            puts("select() error");
            break;
        }
        else if(result == 0)
        {
            puts("Time-out!");
        }
        else
        {//验证发生变化的fd是否为标准输入，若时则从标准输入读取并向控制台输出
            if(FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
        
    }

    return 0;
}
