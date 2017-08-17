#include "../header.h"

int main()
{
    // 锁文件本身
    int fd = open("a.lock", O_RDWR);

    /*加锁*/
    flock(fd, LOCK_EX); // 加锁
    printf("lock ok\n");
    write(fd, "aaa", 3);
    flock(fd, LOCK_UN); // 解锁

    close(fd);

}
