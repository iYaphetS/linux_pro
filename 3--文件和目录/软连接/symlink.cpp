#include "../header.h"


int main()
{
    struct stat stat_buf;

    //abc_symlink->ABC
    //stat("abc_symlink", &stat_buf);//会跟随，得到是ABC的文件属性
    lstat("abc_symlink", &stat_buf);//得到的是abc_symlink的文件属性
    //在命令行stat是不跟随的

    printf("%d\n", (int)stat_buf.st_ino);

    return 0;
}
