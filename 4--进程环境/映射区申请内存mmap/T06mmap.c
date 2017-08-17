#include "../header.h"

int main()
{
    // 在申请一块内存
    void* ptr = mmap(NULL, 
            4096,
            PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANON,
            -1,
            0);

    strcpy((char*)ptr, "hello");

    munmap(ptr, 4096);
}
