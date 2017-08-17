
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
    // 打开文件：打开的是鼠标文件
    int fd = open("/dev/input/mouse0", O_RDONLY); 
    // 
    if(fd < 0)
    {
        perror("open");
        return 0;
    }

    while(1)
    {
        char buf[8];
        // 读取鼠标移动或者点击信息
        int ret = read(fd, buf, sizeof(buf));

        printf("ret is %d\n", ret);
    }
}
