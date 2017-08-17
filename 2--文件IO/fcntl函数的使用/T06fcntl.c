
#include "../header.h"
/*
 * struct file
 * {
 *      int status; // O_WRONLY
 * }
 * */
int main()
{
    int fd = open("a.tmp", O_WRONLY);
    int fd2 = open("a.tmp", O_WRONLY);

    {
        // 给fd增加O_APPEND属性
        int flag;
        flag = fcntl(fd, F_GETFL);
        flag |= O_APPEND;
        fcntl(fd, F_SETFL, flag);
    }

    {
        // 给fd2增加O_APPEND属性
        int flag;
        flag = fcntl(fd2, F_GETFL);
        flag |= O_APPEND;
        fcntl(fd2, F_SETFL, flag);
    }

    write(fd, "hello", 5);
    write(fd2, "world", 5);
}
