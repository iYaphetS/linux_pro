#include "../header.h"

int main()
{
    // 锁文件本身
    int fd = open("a.lock", O_RDWR);

    /*加锁*/
    flock(fd, LOCK_SH); // 加锁
    getchar();

    lseek(fd, 0, SEEK_END);
    write(fd, "bbb", 3);
    flock(fd, LOCK_UN); // 解锁

    close(fd);

}
