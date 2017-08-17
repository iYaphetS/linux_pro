#include "cs_net.h"

char request_buf[] =
"GET / HTTP/1.1\r\n"
"Host: 192.168.19.28:9988\r\n"
"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
"Accept-Language: zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Connection: keep-alive\r\n"
"Upgrade-Insecure-Requests: 1\r\n"
"\r\n";

//给appache服务器发送请求报文，输出服务器的响应
int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);//appache默认端口80
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    myWrite(sock, request_buf, strlen(request_buf));

    char response_buf[8192];

    recv(sock, response_buf, sizeof(response_buf), 0);

    printf("%s\n", response_buf);
    return 0;
}
