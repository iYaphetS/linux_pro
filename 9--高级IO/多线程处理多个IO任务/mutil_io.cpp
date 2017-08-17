#include "../header.h"

int main()
{
    int fd_mice = open("/dev/input/mice", O_RDONLY);
    int fd_keyb = open("/dev/input/event1", O_RDONLY);

    char buf[8];
    char keyb[100];
    int ret;
    while(1)
    {
#if 1
        //一个进程有两个阻塞点，程序就没有办法进行
        //这个进程中有两个read，会相互影响
        ret = read(fd_mice, buf, sizeof(buf));

        printf("0x%02x, 0x%02x, 0x%02x\n", 
                (unsigned char)buf[0],
                (unsigned char)buf[1],
                (unsigned char)buf[2]);
#endif
        ret = read(fd_keyb, keyb, sizeof(keyb));
        printf("ret=%d\n", ret);
      }

    return 0;
}
