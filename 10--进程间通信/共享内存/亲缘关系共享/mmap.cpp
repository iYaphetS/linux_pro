#include "../header.h"
#include <sys/wait.h>
int main()
{
    //栈上堆上的空间子进程都是不会共享的
    char buf[1024] = "aaaaa";
    char *str = (char *)malloc(100);
    strcpy(str, "hehe");
    /*
     *void *mmap(void *addr,
     size_t length,
     int prot,
     int flags,
     int fd,
     off_t offset);
     * */
    void *ptr = mmap(NULL,
            4096,
            PROT_READ|PROT_WRITE,
            MAP_SHARED|MAP_ANON,
            -1,
            0);

    pid_t pid = fork();
    if(pid == 0)
    {
        strcpy(buf, "zzzzz");
        strcpy((char*)ptr, "hello mmap");
        strcpy(str, "heihei");
        return 0;
    }

    //这个读写没有阻塞，需要手动让子进程写
    sleep(1);
    cout << buf << endl;
    cout << (char*)ptr << endl;
    cout << str << endl;
    wait(NULL);
    return 0;
}
