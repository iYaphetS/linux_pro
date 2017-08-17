#include "cs_net.h"


void recvDir(int fd)
{
    //目录长度
    uint32_t len;
    myReadlen(fd, &len);

    //目录名
    char *path = (char *)alloca(len + 1);
    myRead(fd, path, len);
    path[len] = 0;
    mkdir(path, 0777);
}

int recvFile(int fd)
{
    //文件名长度
    uint32_t len;
    myReadlen(fd, &len);

    //文件名
    char *path = (char *)alloca(len + 1);
    myRead(fd, path, len);
    path[len] = 0;

    //文件长度
    myReadlen(fd, &len);
    char buf[4096];
    int filefd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0777);

    while(len > 0)
    {
        int readlen = len < sizeof(buf) ? len : sizeof(buf);
        int ret = myRead(fd, buf, readlen);
        if(ret > 0)
        {
            write(filefd, buf, ret);
            len -= ret;
        }
        else
        {
            break;
        }
    }
    close(filefd);

    if(len == 0)
        return 0;//成功
    return -1;
}


//传输目录文件
int main()
{
    int serverfd = myServer(10000, "127.0.0.1");

    while(1)
    {
        int fd = myAccept(serverfd, NULL, NULL);
        if(fd < 0)
        {
            return -1;
        }
        while(1)
        {
            char flag;
            myRead(fd, &flag, 1);

            if(flag == 'd')
            {
                recvDir(fd);
            }
            else if(flag == 'r')
            {
                if(recvFile(fd) < 0)
                {
                    break;
                }
            }
            else if(flag == 'e')
            {
                break;
            }
        }
        close(fd);
    }
    
    close(serverfd);

    return 0;
}
