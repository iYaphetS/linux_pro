
#include "../header.h"

int main()
{
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "sock_file");//addr.sun_path类似char buf[1024]的buf
    //sock_file只要绑定，就会产生

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock, 20);

    int newfd = accept(sock, NULL, NULL);

    char buf[1518];
    read(newfd, buf, sizeof(buf));

    printf("read data:%s\n", buf);
}
