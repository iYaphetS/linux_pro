#include "../network.h"
int main()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988);
    addr.sin_addr.s_addr = inet_addr("224.0.0.100");

    // 开启广播功能，发送端要开启广播功能
//    int v = 1;
//    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &v, sizeof(v));

    sendto(fd, "hello world", 11, 0, 
            (struct sockaddr*)&addr, sizeof(addr));

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));

    printf("%s\n", buf);

}
