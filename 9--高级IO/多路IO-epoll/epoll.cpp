#include "../header.h"
#include <sys/epoll.h>


int main()
{
    int epollfd = epoll_create(512);


    int fd_mice = open("/dev/input/mice", O_RDONLYi|O_NONBLOCK);
    int fd_keyb = open("/dev/input/event1", O_RDONLY|O_NONBLOCK);

    struct epoll_event ev;
    ev.events = EPOLLIN;//可读

    //添加文件描述符
    ev.data.fd = fd_mice;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_mice, &ev);

    ev.data.fd = fd_keyb;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_keyb, &ev);


    //epoll文件描述符集合使用红黑树实现的，最大数量远大与1024
    //select文件描述符集合使用数组实现的，最大数量1024
    struct epoll_event evout[2];
    char buf[1024];

    while(1)
    {
        int ret = epoll_wait(epollfd, evout, 2, 5000);
        if(ret == 0)//超时
        {
            continue;
        }
        if(ret < 0 && errno == EINTR)//被中断打断
        {
            continue;
        }
        if(ret < 0)//错误
        {
            break;
        }

        //ret > 0
        int i;
        for(i = 0; i < ret; i++)
        {
            int fd = evout[i].data.fd;
            if(read(fd, buf, sizeof(buf)) < 0)
            {
                //close自动将它从epoll中移除
                close(fd);
            }
            if(fd == fd_mice)
            {
                printf("mice message\n");
            }
            else if(fd == fd_keyb)
            {
                printf("keyb message\n");
            }
        }
    }
    return 0;
}
