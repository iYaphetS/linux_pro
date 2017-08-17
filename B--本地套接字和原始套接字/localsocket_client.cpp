
#include "../header.h"


int main()
{

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "sock_file");

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    write(sock, "hello world", 12);//\0--发送过去


}
