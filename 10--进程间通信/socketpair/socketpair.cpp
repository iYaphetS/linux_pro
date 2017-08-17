#include "../header.h"
#include <sys/socket.h>

//类似匿名管道，但是是全双工

int main()
{
    int fd[2];

    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    //管道一端既可以读也可以写
    write(fd[1], "hello", 5);
    //write(fd[0], "hello", 5);

    char buf[256];
    read(fd[0], buf, sizeof(buf));
    //read(fd[1], buf, sizeof(buf));

    cout << buf << endl;
    close(fd[0]);
    close(fd[1]);
    return 0;
}
