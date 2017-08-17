
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
    int fd2 = dup(fd); 

    {
        // 给fd增加O_APPEND属性
        int flag;
        flag = fcntl(fd, F_GETFL);
        flag |= O_APPEND;
        fcntl(fd, F_SETFL, flag);
    }

    {
        int flag;
        flag = fcntl(fd2, F_GETFL);
        if(flag & O_APPEND)
        {
            printf("has O_APPEND\n");
        }
        else
        {
            printf("no O_APPEND\n");
        }
    }
}
