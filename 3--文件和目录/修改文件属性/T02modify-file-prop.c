
#include "../header.h"
int main()
{
    const char* path = "dir2/file2";

    // 修改文件属性
//    chmod(path, 0777|S_ISUID);

    // 检查这个进程是否客户读这个文件
    int ret = access(path, R_OK);
    if(ret == 0)
    {
        printf("可读\n");
    }


    link(path, "dir2/file2-link");
    symlink("file2", "dir2/file2-symlink");

    chown(path, 0, 0);

    struct utimbuf buf;
    buf.actime = 0;
    buf.modtime = 0;
    utime(path, &buf);

    struct timeval tv[2];
    tv[0].tv_sec = 0;
    tv[0].tv_usec = 1;
    tv[1].tv_sec = 0;
    tv[1].tv_usec = 1000;

    utimes(path, tv);
}
