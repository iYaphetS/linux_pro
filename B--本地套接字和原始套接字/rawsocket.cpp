#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include "../header.h"

void handle_tcp(unsigned char *tcp)
{
#if 0
    printf("--------TCP-------\n");
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            unsigned char d = tcp[i*4 + j];
            printf("%02x ", d);
        }
        printf("\n");
    }
#endif

    unsigned char ch = tcp[12];//得到首部长度所占的字节数据
    ch &= 0xf0;//得到4位bit的首部数据
    ch >>= 4;
    int len = ch * 4;//得到TCP头部长度

    char *data = (char *)(tcp + len);
    printf("data is:%s\n", data);

}

void handle_ip(unsigned char *ip)
{
#if 0
    //共5行，每行4个字节
    printf("------IP---------\n");
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            unsigned char d = ip[i*4 + j];
            printf("%02x ", d);
        }
        printf("\n");
    }
#endif
    unsigned char protocal = ip[9];//得到协议号--决定是TCP还是UDP
    int len = (ip[0] & 0x0f);//得到ip的第一个字节的后4位
    len *= 4;//得到ip报文的总长度
    unsigned char *tcp = ip + len;

    if(protocal == 0x11)//UDP
    {

    }
    else if(protocal == 0x06)//TCP
    {
        handle_tcp(tcp);
    }


}

//数据链路层的以太网帧报文解析
void handle_eth(unsigned char *eth_frame)
{
    //报头
    //dst: 00:00:00:00:00:00
    //src: 00:00:00:00:00:00
    //type: 8086
    unsigned char *p = eth_frame;

    printf("dst:%02x:%02x:%02x:%02x:%02x:%02x\n",
           p[0], p[1], p[2], p[3], p[4], p[5]);
    printf("src:%02x:%02x:%02x:%02x:%02x:%02x\n",
           p[6], p[7], p[8], p[9], p[10], p[11]);
    printf("type:%04x\n", *(short *)(p + 12));

}

int main()
{

    //SOCK_RAW---包括以太网帧头和数据
    //SOCK_DGRAM---只包括以太网帧的数据
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sock < 0)
    {
        cout << "0" << endl;
        return 0;
    }
    struct ifreq ifstruct;
    strcpy(ifstruct.ifr_ifrn.ifrn_name, "ens33");

    //绑定网卡
    if(ioctl(sock, SIOCGIFINDEX, &ifstruct) == -1)
    {
        cout << "1" <<endl;
        return 0;
    }

    struct sockaddr_ll addr;//ll  low level
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifstruct.ifr_ifindex;
    addr.sll_protocol = htons(ETH_P_ALL);
    addr.sll_hatype = ARPHRD_ETHER;//硬件类型
    addr.sll_pkttype = PACKET_OTHERHOST;//抓包的类型
    addr.sll_halen = ETH_ALEN;//硬件地址长度，6个字节
    addr.sll_addr[6] = 0;//将硬件地址最后两个字节设置为0
    addr.sll_addr[7] = 0;

    //获取mac地址
    if(ioctl(sock, SIOCGIFHWADDR, &ifstruct) == -1)
    {
        cout << "2" << endl;
        return 0;
    }
    //得到抓包的模式
    if(ioctl(sock, SIOCGIFFLAGS, &ifstruct) < 0)
    {
        cout << "3" << endl;
        return 0;
    }

    //设置混杂模式，这样就可将数据链路的数据得到
    ifstruct.ifr_ifru.ifru_flags |= IFF_PROMISC;
    if(ioctl(sock, SIOCGIFFLAGS, &ifstruct) == -1)
    {
        cout << "4" << endl;
        return 0;
    }

    //绑定地址
    if(bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) == -1)
    {
        cout << "5" << endl;
        return 0;
    }
    //ioctl(sock, SIOCGIFHWADDR, &ifstruct);
    char  buf[1518];

    while(1)
    {
        read(sock, buf, sizeof(buf));
        //printf("recv data:%s\n", buf);
        //handle_eth((unsigned char *)buf);
        unsigned short eth_type = *(unsigned short *)(buf + 12);
        eth_type = ntohs(eth_type);//将网络字节序列转换为本地字节序列
        if(eth_type == 0x0800)//ip 报文
        {
            handle_ip((unsigned char *)(buf + 14));
        }

    }

}
