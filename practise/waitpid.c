//销毁僵尸进程，不阻塞
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
    int status;
    pid_t pid = fork();

    if(pid == 0)//child
    {
        // sleep(5);
        return 24;
        // exit(1);
        // printf("i am child");
    }
    else
    {
        printf("Child PID: %d \n", pid);
        // wait(&status);
        sleep(5);//因为是非阻塞的，所以要延迟几秒等子进程销毁完毕
        waitpid(-1, &status, WNOHANG);
        // while(!waitpid(-1, &status, WNOHANG))
        // {
        //     sleep(3);
        //     puts("sleep 3sec.\n");
        // }
        if(WIFEXITED(status))
            printf("child send : %d \n", WEXITSTATUS(status));
            
    }
    
    return 0;
}