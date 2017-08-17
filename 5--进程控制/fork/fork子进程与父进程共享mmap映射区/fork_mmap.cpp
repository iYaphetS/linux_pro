#include "../header.h"


//内存映射mmap
//fork产生的子进程会与父进程共享一块mmap申请的内存
//而malloc申请的内存不会
int main()
{

    //void *mmap(void *addr,//起始地址
    //           size_t length,//大小
    //           int prot,//读写执行权限
    //           int flags,//
    //           int fd,//文件描述符
    //           off_t offset);//起始偏移量
    char *ptr = (char *)mmap(NULL,
                    4096,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED|MAP_ANON,
                    -1,
                    0);
    char *str = (char *)malloc(1024);

    strcpy(ptr, "hello a");
    strcpy(str, "hello b");

    pid_t pid = fork();
    if(pid == 0)
    {
        strcpy(ptr, "AAA");
        strcpy(str, "BBB");
    }
    else
    {
        sleep(1);//让子进程的设置生效
        cout << ptr << endl;
        cout << str << endl;

    }

    return 0;
}
