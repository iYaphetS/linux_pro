#include "../header.h"

//在外面创建一个管道文件a.fifo
int main()
{
    //打开管道文件，管道的读端和写端都打开了
    int fd = open("a.fifo", O_WRONLY);

    if(fd < 0)
    {
        perror("open");
        return -1;
    }

    //从标准输入中输入数据
    char buf[1024];
    fgets(buf, sizeof(buf), stdin);
    
    write(fd, buf, strlen(buf));
    return 0;
}
