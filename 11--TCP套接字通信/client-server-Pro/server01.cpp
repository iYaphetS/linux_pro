#include "cs_net.h"

//传输普通文件
int main()
{
    int serverfd = myServer(9988, "127.0.0.1");
    if(serverfd == -1)
    {
        cout << "create server fail" << endl;
        return -1;
    }

    while(1)
    {
        int fd = myAccept(serverfd, NULL, NULL);
        if(fd == -1)
        {
            cout << "accept error" << endl;
            return -1;
        }

        //读传输过来的文件名的长度
        uint32_t len;
        myReadlen(fd, &len);

        //得到文件名
        char *path = (char *)alloca(len+1);
        path[len] = 0;
        myRead(fd, path, len);

        //创建文件
        int filefd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0777);

        //得到文件的大小
        myReadlen(fd, &len);
        char buf[4096];
        while(len > 0)
        {
            int readlen = len > sizeof(buf) ? sizeof(buf) : len;
            int ret = myRead(fd, buf, readlen);

            if(ret > 0)
            {
                write(filefd, buf, ret);
                len -= ret;
            }
            else if(ret <= 0)
            {
                break;
            }
        }
        close(filefd);
        close(fd);
        //free(path);
    }
    close(serverfd);
}
