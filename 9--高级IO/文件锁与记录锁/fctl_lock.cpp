#include "../header.h"

//文件锁是对文件整体全部上锁，当一个文件上锁后还没有解锁，另一个程序操作就得阻塞
//而记录锁是对文件的一部分进行上锁，当一个程序对文件的某一部分上锁，不会阻塞另一个程序操作文件的其他部分
int main()
{
    int fd = open("a.txt", O_RDWR);

    struct flock lock;
    lock.l_type = F_WRLCK;//锁的类型
    lock.l_whence = SEEK_SET;//从哪里开始
    lock.l_start = 1024;//开始位置的偏移长度,可为0
    lock.l_len = 1024;//锁住的文件长度，可为整个文件长度

    //F_SETLKW--等待锁，操作已经上锁的文件，等待上一个进程关闭锁，这个进程才能上锁
    //F_SETLK--非等待锁，操作已经上锁的文件，直接返回，不会等待
    fcntl(fd, F_SETLKW, &lock);

    printf("get lock\n");
    getchar();

    //解锁
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    return 0;
}
