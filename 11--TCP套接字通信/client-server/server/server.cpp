#include "../../header.h"
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    //创建一个socket对象
    //返回值：文件描述符
    //第一个参数：这个socket是用AF_INET进行网络通信
    //第二个参数：文件的传输形式，这个是流套接字也就是TCP，如果是UDP，应为SOCK_DGRAM
    //第三个参数：IP协议，默认写0
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //指定地址和端口--封装成的地址结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;//地址的网络类型
    //IP地址，有函数inet_addr转成整数地址
    //如果监听任何网口
    //addr.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY(0)表示任何网口
    //可以写具体ip地址也可以写指向具体地址的环回地址127.0.0.1
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //端口，要大于1000，不同平台间通信需要大小端字节序转换
    addr.sin_port = htons(20000);

    //将这个地址和fd绑定起来
    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return 0;
    }
    //只有服务器监听后，客户端才能和服务器链接起来
    cout << "please press any key to start listen" << endl;
    getchar();
    //第二个参数：能够同时链接的客户端数量
    listen(fd, 10);

    //检查是否有客户端链接我，这个调用是个阻塞调用，没有人链接将阻塞在这里
    int newfd = accept(fd, NULL, NULL);
    if(newfd < 0)
    {
        perror("accept");
        return 0;
    }

    //得到文件名
    char name_buf[1024] = {0};
    read(newfd, name_buf, sizeof(name_buf));
    char *recvfile = strtok(name_buf, "\n");

    //读数据
    int recv_fd = open(recvfile, O_WRONLY|O_CREAT|O_TRUNC,0777);
    //读客户端发来的数据，阻塞的
    char buf[4096];
    while(1)
    {
        int ret = read(newfd, buf, sizeof(buf));
        if(ret <= 0)
            break;
        write(recv_fd, buf, ret);
    }
    cout << "接受完毕" << endl;

    close(newfd);
    close(fd);


    return 0;
}
