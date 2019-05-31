//销毁僵尸进程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
typedef __pid_t pid_t;

int main(int argc, char** argv)
{
    int status;
    pid_t pid = fork();

    if(pid == 0)//child
    {
        exit(1);
        // printf("i am child");
    }
    else
    {
        printf("Child PID: %d \n", pid);
        pid = fork();
        if(pid == 0)
            exit(7);
        else
        {
            printf("Child PID: %d \n", pid);
            wait(&status);
            if(WIFEXITED(status))
                printf("child send one: %d \n", WEXITSTATUS(status));
            
            wait(&status);
            if(WIFEXITED(status))
                printf("child send two: %d \n", WEXITSTATUS(status));
            
            sleep(30);
        }
        
    }
    
    return 0;
}