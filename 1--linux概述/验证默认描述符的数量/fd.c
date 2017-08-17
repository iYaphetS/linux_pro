
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>

// yanzheng wenjian miaoshufu shuliang
int main()
{
    for(int i=0; ;++i)
    {
        int fd = open("fd.c", O_RDONLY);
        if(fd < 0)
        {
            printf("open error i is %d\n", i);
            break;
        }
        else
        {
            printf("fd is %d\n", fd);
        }
    }
}
