
#include "../header.h"
int main()
{
    int fd = open("a.tmp", O_WRONLY);

    // 拷贝fd到1的位置，导致后面的写
    // 写到fd指向的文件
    dup2(fd, 1);
    close(fd);


    // 以下省略了三千行代码
    // 就是描述符为1的文件进行写
    printf("hello world\n");
}
