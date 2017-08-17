#include "../header.h"


int main()
{
    pid_t pid = fork();

    //关于是先执行子进程，还是父进程，不确定的

    //当pid等于0时，调用了fork产生的子进程
    if(pid == 0)
    {
        printf("this is child\n");
        printf("child_pid = %d, parent_pid = %d\n", getpid(), getppid());
    }
    //当pid大于0时，pid其实就是子进程的pid
    else
    {
        sleep(1);
        printf("this is parent\n");
        printf("child_pid = %d, parent_pid = %d\n", pid, getpid());
    }

    return 0;
}
