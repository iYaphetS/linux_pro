
#include "../network.h"
int main()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
        return -1;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0) return -1;

    // 注册组播，能接收组播报文
    struct ip_mreq mreq;                                    /*加入多播组*/
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.100"); /*多播地址*/
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/
    /*将本机加入多播组*/
    int err = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));

    if (err < 0)
    {
        printf("haha\n");
        return -1;
    }

    char buf[4096];
    memset(buf, 0, sizeof(buf));
    //    read(fd, buf, sizeof(buf));

    struct sockaddr peer_addr;
    socklen_t addrlen = sizeof(peer_addr);
    recvfrom(fd, buf, sizeof(buf), 0, &peer_addr, &addrlen);
    printf("%s\n", buf);

    strcpy(buf, "你好");
    sendto(fd, buf, strlen(buf), 0, &peer_addr, sizeof(peer_addr)); 

}
