#include "../header.h"


int main(int argc, char *argv[])
{
    const char *src_filename = argv[1];
    const char *dst_filename = argv[2];

    FILE *fp1 = fopen(src_filename, "r");
    FILE *fp2 = fopen(dst_filename, "w");

    chmod(src_filename, 0744);
    chown(src_filename, 0, 0);
    if(fp1 == NULL)
    {
        perror("open");
        return -1;
    }
    if(fp2 == NULL)
    {
        perror("open");
        return -2;
    }

    //拷贝文件
    char buf[4096];
    while(1)
    {
        int ret = fread(buf, 1, sizeof(buf), fp1);
        if(ret <= 0)
            break;

        fwrite(buf, ret, 1, fp2);
    }

    //得到src文件属性
    struct stat stat_buf;
    stat(src_filename, &stat_buf);

    //拷贝文件的读写执行权限
    chmod(dst_filename, stat_buf.st_mode & 0777);

   // cout << stat_buf.st_mode << endl;

    //属性更改时间不能设置
    struct timeval tv[2];
    tv[0].tv_sec = stat_buf.st_atime;//访问时间
    tv[0].tv_usec = 0;
    tv[1].tv_sec = stat_buf.st_mtime;//修改时间
    tv[1].tv_usec = 0;
    utimes(dst_filename, tv);

    //cout << stat_buf.st_uid << " " << stat_buf.st_gid << endl;
    //修改这个需要root权限，所以执行的时候加上sudo
    //拷贝文件所属用户和组
    chown(dst_filename, stat_buf.st_uid, 
            stat_buf.st_gid);

    fclose(fp1);
    fclose(fp2);

    return 0;
}
