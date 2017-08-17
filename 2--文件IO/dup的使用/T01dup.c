
#include "../header.h"

int main()
{
    int fd = open("a.tmp", O_RDWR);

    // 将fd复制fd2，fd2也是一个文件描述符，和fd共享一个文件结构体
    int fd2 = dup(fd);
    // dup和open一样，系统为它自动分配文件描述符
    // 分配的原则也是 最小可用的文件描述符

    write(fd, "hello", 5);
    write(fd2, "world", 5);

    close(fd);
    close(fd2);
}
