#include "../header.h"

//多线程解决多路IO问题

void *thread_mice(void *ptr)
{
    int fd_mice = open("/dev/input/mice", O_RDONLY);
    char buf[8];
    int ret;
    while(1)
    {
        //一个进程有两个阻塞点，程序就没有办法进行
        //这个进程中有两个read，会相互影响
        ret = read(fd_mice, buf, sizeof(buf));
        printf("0x%02x, 0x%02x, 0x%02x\n", 
                (unsigned char)buf[0],
                (unsigned char)buf[1],
                (unsigned char)buf[2]);
      }

}

void *thread_keyb(void *ptr)
{
    int fd_keyb = open("/dev/input/event1", O_RDONLY);
    char keyb[100];
    int ret;
    while(1)
    {
        //一个进程有两个阻塞点，程序就没有办法进行
        //这个进程中有两个read，会相互影响
        ret = read(fd_keyb, keyb, sizeof(keyb));
        printf("ret=%d\n", ret);
      }

}

int main()
{
    
    pthread_t tid_mice, tid_keyb;
    
    pthread_create(&tid_mice, NULL, thread_mice, NULL);
    pthread_create(&tid_keyb, NULL, thread_keyb, NULL);

    pthread_join(tid_mice, NULL);
    pthread_join(tid_keyb, NULL);
    return 0;
}
