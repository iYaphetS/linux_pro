#include "cs_net.h"

//客户端 传输文件的形式
//./a.out src dst ip port
int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        cout << "参数不足" << endl;
    }
    signal(SIGPIPE, SIG_IGN);
    const char *src = argv[1];
    const char *dst = argv[2];
    const char *ip = argv[3];
    const char *port = argv[4];

    //创建客户端（并链接服务器）
    int fd = myClient(atoi(port), ip);
    if(fd < 0)
    {
        cout << "create client fail" << endl;
        return -1;
    }
    
    //发送文件名的长度
    uint32_t len = strlen(dst);
    myWritelen(fd, len);
    //发送文件名
    myWrite(fd, dst, strlen(dst));

    //获取要发送的源文件的文件大小
    int filelen = getfilesize(src);
    //发送文件大小
    myWritelen(fd, filelen);

    //打开源文件
    int filefd = open(src, O_RDONLY);
    //发送文件内容
    char buf[4096];
    while(1)
    {
        int ret = read(filefd, buf, sizeof(buf));
        if(ret <= 0)
        {
            break;
        }

        myWrite(fd, buf, ret);

    }
    
    return 0;
}
