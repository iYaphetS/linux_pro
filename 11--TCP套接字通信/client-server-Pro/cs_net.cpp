#include "cs_net.h"

//用于创建server
int myServer(uint16_t port, const char *ip)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("bind");
        close(fd);
        return -1;
    }
    listen(fd, 250);

    return fd;
}


int myAccept(int server_fd, struct sockaddr *addr, socklen_t* addrlen)
{
    while(1)
    {
        int fd = accept(server_fd, addr, addrlen);
        if(fd < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            perror("accept");
            return -1;
        }
        return fd;
    }
}

//client
int myClient(uint16_t port, const char *ip)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if(ret < 0)
    {
        perror("connect");
        return -1;
    }
    return fd;

}

//由于TCP传输是基于流形式的，就是要发的所有数据都是连在一起传输的，就导致在读数据的时候可能得到文件某个信息的数据过多或者过少，也就是粘包问题
//处理就需要先传输这个信息的大小，然后根据大小准确读到需要的信息
int myRead(int fd, char buf[], int len)
{
    int readlen = 0;

    while(readlen != len)
    {
        int ret = read(fd, buf + readlen, len - readlen);
        if(ret > 0)
        {
            readlen += ret;
        }
        else if(ret < 0)
        {
            if(errno == EINTR)
                continue;
            return ret;
        }
        else
        {
            break;
        }
    }
    return readlen;
}

//len作为输入
int myWrite(int fd, const char buf[], int len)
{
    int writelen = 0;
    while(writelen != len)
    {
        int ret = write(fd, buf + writelen, len - writelen);
        if(ret > 0)
        {
            writelen += ret;
        }
        else if(ret < 0)
        {
            if(errno == EINTR)
                continue;
            else
                break;
        }
    }
    return writelen;
}

//len既做输入也做输出
int myReadlen(int fd, uint32_t *len)
{
    int ret = myRead(fd, (char *)len, sizeof(uint32_t));
    *len = ntohl(*len);
    return ret;
}

int myWritelen(int fd, uint32_t len)
{
    len = htonl(len);
    int ret = myWrite(fd, (char *)&len, sizeof(len));
    return ret;
}
int getfilesize(const char *path)
{
    int filefd = open(path, O_RDONLY);
    struct stat stat_buf;
    stat(path, &stat_buf);
    int filelen = stat_buf.st_size;
    return filelen;

}
