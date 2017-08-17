#include "../header.h"


int main()
{
    //fd打开的一块内存
    //"/share_mem"名字有格式，以/开始的字符串，且字符串中不能有/
    int fd = shm_open("/share_mem", O_RDWR|O_CREAT|O_EXCL, 0777);
    if(fd < 0)//文件存在，创建失败
    {
        fd = shm_open("/share_mem", O_RDWR, 0777);
    }
    {
        ftruncate(fd, 4096);
    }
    void *ptr = mmap(NULL,
            4096,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            0);

    char buf[1024];
    strcpy((char *)ptr, "hello mmap_mem");

    cout << "写入成功" << endl;
    //getchar();
    close(fd);
    shm_unlink("/share_mem");
    munmap(ptr, 4096);
    return 0;
}
