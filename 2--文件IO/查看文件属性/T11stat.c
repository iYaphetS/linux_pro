#include "../header.h"
int main()
{
    struct stat buf;
    int ret = stat(".", &buf);
    if(ret < 0)
    {
        printf("stat error\n"); // 文件不存在
        return 0;
    }

    printf("ino is %d\nmode=0x%x\nsize=%d\nblksize=%d\nblocks=%d\n", 
            (int)buf.st_ino, (int)buf.st_mode, (int)buf.st_size, (int)buf.st_blksize, (int)buf.st_blocks);


    if(S_ISREG(buf.st_mode))
    {
        printf("this is regular file\n");
    }
    else if(S_ISDIR(buf.st_mode))
    {
        printf("this is dir\n");
    }

    // 判断，拥有这个文件的账户，是否可以读这个文件
    if(S_IRUSR & buf.st_mode)
    {
        printf("user can read\n");
    }
}
