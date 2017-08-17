#include "../../header.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cout << "arg error" << endl;
    }
    //当接受到SIGPIPE信号，就忽视它
    signal(SIGPIPE, SIG_IGN);
    int fd = socket(AF_INET,SOCK_STREAM, 0);


    struct sockaddr_in addr;
    addr.sin_family = AF_INET;    
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(20000);

    cout << "please press any key to start connect" << endl;
    getchar();
    if(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
    }

    //如果write时，对方关闭了sockt，会收到SIGFIFO信号（管道破裂）

    int send_fd = open(argv[1], O_RDONLY);
    struct stat stat_buf;
    stat(argv[1], &stat_buf);
    int file_size = stat_buf.st_size;

    //传输文件名
    char filename[256] = {0};
    sprintf(filename, "%s\n", argv[1]);
    write(fd, filename, strlen(filename));
    char buf[4096];
    while(file_size > 0)
    {
        int readlen = file_size < 4096 ? file_size : 4096;
        int ret = read(send_fd, buf, readlen);
        if(ret < 0)
        {
            break;
        }
        write(fd, buf, ret);

        file_size -= ret;
    }

    close(fd);
    return 0;
}
