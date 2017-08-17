#include "cs_net.h"


int sendFile(const char *dst, const char *src, int fd)
{
    //发送文件标志
    myWrite(fd, "r", 1);
    //发送文件名长度，文件名
    myWritelen(fd, strlen(dst));
    myWrite(fd, dst, strlen(dst));

    //发送文件大小
    myWritelen(fd, getfilesize(src));
    int filefd = open(src, O_RDONLY);
    char buf[4096];
    while(1)
    {
        int ret = read(filefd, buf, sizeof(buf));
        if(ret == 0)
            break;
        myWrite(fd, buf, ret);
    }
    close(filefd);
}
int sendDir(const char *dst, const char *src, int fd)
{
    //发送目录标志
    myWrite(fd, "d", 1);
    //发送文件名长度和文件名
    myWritelen(fd, strlen(dst));
    myWrite(fd, dst, strlen(dst));

    char *bufSrc = (char *)malloc(1024);
    char *bufDst = (char *)malloc(1024);

    //读目录
    DIR *dir = opendir(src);
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL)
    {
        if(entry->d_name[0] == '.')
        {
            continue;
        }
        sprintf(bufDst, "%s/%s", dst, entry->d_name);
        sprintf(bufSrc, "%s/%s", src, entry->d_name);
        if(entry->d_type == DT_REG)
        {
            sendFile(bufDst, bufSrc, fd);
        }
        else if(entry->d_type == DT_DIR)
        {
            sendDir(bufDst, bufSrc, fd);
        }
    };
    free(bufDst);
    free(bufSrc);
    closedir(dir);
    return 0;


}

int main(int argc, char *argv[])
{
    const char *src = argv[1];
    const char *dst = argv[2];
    const char *ip = argv[3];
    const char *port = argv[4];

    int fd = myClient(atoi(port), ip);
    if(fd < 0)
    {
        cout << "create client fail" << endl;
    }

    sendDir(dst, src, fd);
    //发送结束标志
    myWrite(fd, "e", 1);

    return 0;
}
