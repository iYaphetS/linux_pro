#include "../header.h"

//设置非阻塞属性
void set_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

int main()
{
    int fd = open("/dev/input/mice", O_RDONLY|O_NONBLOCK);

    //int fd = open("/dev/input/mice", O_RDONLY);
    //set_nonblock(fd);

    char buf[8];

    while(1)
    {
        //在没有设置文件的非阻塞属性前，read是阻塞的
        int ret = read(fd, buf, sizeof(buf));
        if(ret < 0)
        {
            if(errno == EAGAIN)
            {
                cout << "EAGAIN" << endl;
            }
            perror("read");
        }
        printf("%02x, %02x, %02x\n", 
                (unsigned char)buf[0],
                (unsigned char)buf[1],
                (unsigned char)buf[2]);

    }

    return 0;
}
