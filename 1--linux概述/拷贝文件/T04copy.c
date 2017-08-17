
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    const char* srcfilename = argv[1];
    const char* dstfilename = argv[2];

    int fd_src = open(srcfilename, O_RDONLY);
    if(fd_src < 0)
    {
        perror("open src");
        return 0;
    }

    int fd_dst = open(dstfilename, O_WRONLY|O_CREAT|O_EXCL, 0777);
    if(fd_dst < 0)
    {
        // file exist
        fd_dst = open(dstfilename, O_WRONLY|O_TRUNC);
        if(fd_dst < 0)
        {
            perror("open dst");
            return 0;
        }
    }


    // why 4K ...
    // system call
    // 
    char buf[4096];
    while(1)
    {
        int readlen = read(fd_src, buf, sizeof(buf));
        if(readlen == 0)
            break;
        write(fd_dst, buf, readlen);
    }

//    close(fd_src);
//    close(fd_dst);
    
    return 0;
}

