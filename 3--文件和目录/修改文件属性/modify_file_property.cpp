#include "../header.h"


int main()
{
    const char *path = "ABC/a.txt";

    //修改文件读写执行权限
    chmod(path, 0777|S_ISUID);

    //检查是否有读权限
    int ret = access(path, R_OK);
    if(ret == 0)
    {
        cout << "可读" << endl;
    }

    //为这个文件添加硬链接
    link(path, "ABC/a.txt_link");
    //为这个文件添加软链接
    symlink("a.txt", "ABC/a.txt_symlink");
    
    //改变这个文件的所属用户和组
    chown(path, 0, 0);//都设置为root

    //修改文件的访问时间和修改时间（修改文件内容）
    //但是更改时间（修改文件的属性）是不能修改的
    struct utimbuf buf;
    //设置的时间相对与unix时间戳流逝的时间
    buf.actime = 0;
    buf.modtime = 0;
    utime(path, &buf);


    //修改对应的毫秒和微秒
    struct timeval tv[2];
    //设置访问时间
    tv[0].tv_sec = 1;
    tv[0].tv_usec = 1000;
    //设置修改时间
    tv[1].tv_sec = 2;
    tv[1].tv_usec = 2000;

    utimes(path, tv);

    return 0;
}
