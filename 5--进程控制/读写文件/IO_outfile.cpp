#include "../header.h"
#include <arpa/inet.h>
struct student
{
    char *name;
    int age;
};
int main()
{
    struct student s;
    int fd = open("s.info", O_RDONLY);

    int temp;
    read(fd, &temp, sizeof(temp));
    //将网络字节序-----主机字节序
    s.age = ntohl(temp);

    int len;
    read(fd, &temp, sizeof(temp));
    len = ntohl(temp);

    s.name = (char *)malloc(len + 1);
    s.name[len] = '\0';
    read(fd, s.name, len);
    
    printf("name:%s\nage:%d\n", s.name, s.age);

    close(fd);
    return 0;
}
