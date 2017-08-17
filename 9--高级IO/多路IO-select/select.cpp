#include "../header.h"



int main()
{
    uint32_t a = 1;
    uint32_t ret = 0;
    ret |= (a<<1);
    cout << ret << endl;
    int fd_mice = open("/dev/input/mice", O_RDONLY);
    int fd_keyb = open("/dev/input/event1", O_RDONLY);

    /*
     *int select(int nfds,//最大文件描述符+1 
     fd_set *readfds,//可读的文件描述符集合
     fd_set *writefds,//可写的文件描述符集合
     fd_set *exceptfds,//异常文件描述符集合
     struct timeval *timeout);//等待的时间限制，如果为NULL
     将是永久等待

     //三个文件描述符集合,及时间限制的参数既做输入参数，也做输出参数
     //----也就是说：当文件描述符集合中有消息被读了，这个文件描述符被删除
    //等待了1秒有了消息，那么timeout就等于timeout-1了

    //select是一个阻塞函数（有时间限制），它往下执行的条件：
    //1. 这些集合中有文件描述符
    //2. 如果没有描述符，超过了等待时间     
    
    //select的返回值
    //1.返回值为0时，表示超时
    //2.返回值为-1时且错误码为EINTR，表示select阻塞被中断打断
    //3.返回值为-1时，表示失败
     * */   
    while(1)
    {
        int fds = fd_mice;
        if(fds < fd_keyb)
        {
            fds = fd_keyb;
        }

        fd_set readfds;
        //如何将fd_key, fd_mice添加到readfds
        FD_ZERO(&readfds);
        FD_SET(fd_mice, &readfds);
        FD_SET(fd_keyb, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 5;//5秒
        timeout.tv_usec = 0;//0微妙

        int ret = select(fds + 1, &readfds, NULL, NULL, &timeout);

        if(ret == 0)
        {
            continue;
        }
        if(ret == -1 && errno == EINTR)
        {
            continue;
        }
        if(ret == -1)
        {
            break;
        }

        char buf[1024];
        if(FD_ISSET(fd_mice, &readfds))
        {
            //注意只有将fd_mice的内容读走，下次才不会读到，不然会一直读到这个消息
            //读多少内容就会将内容清除
            //内核文件不同与磁盘上的文件
            //读走的内容就不在
            read(fd_mice, buf, sizeof(buf));
            printf("mice message\n");
        }
        if(FD_ISSET(fd_keyb, &readfds))
        {
            read(fd_keyb, buf, sizeof(buf));
            printf("keyb message\n");
        }
    }


    
    return 0;
}
