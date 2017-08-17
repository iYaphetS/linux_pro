
#include "../header.h"

int main()
{
    // 打开了3号文件描述符号 3-->a.tmp
    int fd = open("a.tmp", O_WRONLY);
    // 将1号文件描述符，拷贝到4号文件描述符，1-->终端，4-->终端
    int fd_backup = dup(1);  // 备份标准输出的文件结构体

    // write(1, ...) --> 终端
    printf("hello to screen1\n");

    // 1 --> a.tmp
    dup2(fd, 1); // 将标准输出，重定向到fd所指的文件
    close(fd);
    // write(1, ...) 输出到a.tmp
    printf("hello to file\n");

    // 把文件4拷贝到1，所以 1-->终端
    dup2(fd_backup, 1);
    close(fd_backup);

    // write(1, ...) 写到终端
    printf("hello to screen2\n");

    return 0;

}
