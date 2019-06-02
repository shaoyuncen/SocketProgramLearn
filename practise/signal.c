/*包含signal % sigaction两种*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



void timeout(int sig)//定义信号处理函数， Handler
{
    if(sig == SIGALRM)
        puts("Time out!\n");
    alarm(2);//2s后产生SIGALRM信号   
}

void keycontrol(int sig)
{
    if(sig == SIGINT)
        puts("CTRL+C pressed\n");
}

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if(WIFEXITED(status))
    {
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

//信号处理消灭僵尸进程
int main(int argc, char** argv)
{
    pid_t pid;
    //设置信号处理函数
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask); //置0
    act.sa_flags = 0;

    sigaction(SIGCHLD, &act, 0);//子进程终止则调用act中的信号处理函数->read_childproc

    pid = fork();

    if(pid == 0)
    {//子进程
        puts("Hi, i am child 1");
        sleep(5);
        puts("child 1 wake up.");
        return 12;
    }
    else
    {//父进程
        printf("Child proc id: %d \n", pid);
        pid = fork();
        if(pid==0)
        {
            puts("Hi i am child 2");
            sleep(11);
            puts("child 2 wake up.");
            exit(24);    
        }
        else
        {
            int i;
            printf("Child proc id: %d \n", pid);
            for(int i=0;i<5;i++)
            {
                puts("wait......");
                sleep(5);
            }
    
        }
        
    }

    
    

    return 0;
}

// //sigaction函数
// int main(int argc, char** argv)
// {
//     int i;
//     struct sigaction act;
//     act.sa_handler = timeout;
//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;
//     sigaction(SIGALRM, &act, 0);
//     alarm(2);

//     for(int i=0;i<3;++i)
//     {
//         puts("wait...");
//         sleep(100);
//     }

//     return 0;
// }
// //signal函数
// int main(int argc, char** argv)
// {
//     int i;
//     signal(SIGALRM, timeout);
//     signal(SIGINT, keycontrol);
//     alarm(2);

//     for(int i=0;i<3;++i)
//     {
//         puts("wait......");
//         sleep(100);
//     }

//     return 0;
// }