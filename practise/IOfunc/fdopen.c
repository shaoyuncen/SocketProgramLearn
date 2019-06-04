#include <stdio.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    FILE* fp;
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC);
    if(fd == -1)
    {
        fputs("file open error()", stdout);
        return -1;
    }
    fp = fdopen(fd, "w"); //将open函数创建的fd转换为写模式的FILE指针
    fputs("Network C programming \n", fp);
    fclose(fp);

    return 0;
}