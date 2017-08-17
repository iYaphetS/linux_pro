#include "../header.h"


int main()
{
    pid_t pid = fork();

    if(pid == 0)
    {
        //子进程脱离组，单独创建一个会话
        setsid();//进程组组长，不能单独创建会话，因为这个原来组进程会是新会话的组长，这个组的组id会与原来的那个组的id冲突
        sleep(100);//终端（父进程结束）没有了，无法捕获用户的输入信息
    }

    return 0;
}
