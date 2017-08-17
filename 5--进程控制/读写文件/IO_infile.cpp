#include "../header.h"
#include <arpa/inet.h>
struct student
{
    char *name;
    int age;
};
int main()
{
    char buf[2048];

    struct student s;


    printf("please input name:");
    fgets(buf, sizeof(buf), stdin);
    int len = strlen(buf);
    buf[len - 1] = 0;
    s.name = strdup(buf);
#if 0 
    相当于
    s.name = malloc(len);
    memset(s.name, 0, len);
    memcpy(s.name, buf, len - 1);
#endif
    printf("please input age:");
    fgets(buf, sizeof(buf), stdin);
    len = strlen(buf);
    buf[len - 1] = 0;
    s.age = atoi(buf);


    //将信息写到文件中
    int fd = open("s.info", O_WRONLY|O_CREAT|O_TRUNC, 0777);
    len = strlen(s.name);

    int temp;
    //将一个整数有host（主机本身字节序）------转化-----net（网络字节序，一般都是大端字节序）的（l长整数）
    //如果主机字节序与net相同，将不起作用
    temp = htonl(s.age);
    write(fd, &temp, sizeof(temp));

    //名字大小便于读文件
    temp = htonl(len);
    write(fd, &temp, sizeof(temp));
    write(fd, s.name, len);

    
    close(fd);
    return 0;
}
