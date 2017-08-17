#include "../header.h"
#include <sys/wait.h>


void file_copy(const char *src, const char * dst, int pos, int len)
{
    cout << pos << endl; 
    FILE *fp1 = fopen(src, "r");
    FILE *fp2 = fopen(dst, "r+");//注意打开方式,用"w"打开文件，会把以前的以前的文件截断(清零)

    fseek(fp1, pos, SEEK_SET);
    fseek(fp2, pos, SEEK_SET);
    char buf[4096];
    int size = sizeof(buf);
    while(len)
    {
        int read_len = len < size ? len : size;

        int ret = fread(buf, 1, read_len, fp1);

        fwrite(buf, ret, 1, fp2);

        len -= ret;
        

    }

    fclose(fp1);
    fclose(fp2);

}

int main(int argc, char *argv[])
{

    if(argc != 4)
    {
        cout << "参数不足,请按下面格式列出：" << endl;
        cout << argv[0] << " [process_count][src_file] [dst_file]" <<  endl;
        return -1;
    }

    int process_count = atoi(argv[1]);
    if(process_count <= 0)
    {
        cout << "process_count error" << endl;
        return -2;
    }

    const char *src_file = argv[2];
    const char *dst_file = argv[3];

    struct stat stat_buf;
    stat(src_file, &stat_buf);

    int filelen = stat_buf.st_size;
    if(filelen < 0)
    {
        cout << "file error" << endl;
    }

    int fd = open(dst_file, O_CREAT|O_WRONLY, 0777);
    close(fd);
    truncate(dst_file, filelen);

    chmod(dst_file, stat_buf.st_mode & 0777);
    chown(dst_file, stat_buf.st_uid, stat_buf.st_gid);


    int average = filelen / process_count;
    int i;
    for(i = 0; i < process_count - 1; ++i)
    {
        pid_t pid = fork();

        if(pid == 0)
        {
            int pos = average * i;
            file_copy(src_file, dst_file, pos, average);
            return 0;
        }

    }

    int pos = average * i;
    file_copy(src_file, dst_file, pos, filelen - pos);

    for(i = 0; i < process_count - 1; i++)
    {
        wait(NULL);
    }

    return 0;
}
