
#include "../header.h"
int main()
{
    int fd = open("a.tmp", O_RDWR);

    int fd2 = dup2(fd, 2);
    printf("fd2 is %d\n", fd2);

    // 如果拷贝的目标已经打开了
    // 那么dup2会导致目标的文件描述符所指的文件，自动关闭，然后再拷贝
}
