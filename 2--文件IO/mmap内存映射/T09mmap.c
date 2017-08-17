
#include "../header.h"

int main()
{
    // 映射部分的内容，应该文件本来就有的内容
    // 映射不会增长文件尺寸
    //
    void* addr = NULL;  // 指定为null即可
    size_t length = 4096;      // 映射长度 
    int prot = PROT_READ|PROT_WRITE;        // protect
    int flags = MAP_PRIVATE;     // MAP_SHARED才会将内存体现在硬盘, MAP_PRIVATE不会将数据保存到硬盘，只有shared才可以
    int fd = open("a.tmp", O_RDWR);
    off_t offset = 0;       // 从文件的哪部分开始映射

    char* ptr = (char*)mmap(
            addr, 
            length, 
            prot,
            flags,
            fd,
            offset);
    close(fd);

    strcpy(ptr, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    
    munmap(ptr, length);
}
