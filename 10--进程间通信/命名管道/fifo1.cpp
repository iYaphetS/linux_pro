#include "../header.h"

//在外面创建一个管道文件a.fifo
int main()
{
    //打开管道文件，管道的读端和写端都打开了
    int fd = open("a.fifo", O_RDONLY);

    if(fd < 0)
    {
        perror("open");
        return -1;
    }

    char buf[1024];
    //因为没有数据写入，会一直阻塞在这里
    int ret = read(fd, buf, sizeof(buf));
    cout << buf << endl;    
    return 0;
}
