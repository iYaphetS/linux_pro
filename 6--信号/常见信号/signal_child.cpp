#include "../header.h"
#include <signal.h>
#include <sys/wait.h>
void handle(int v)
{
    if(v == SIGINT)
    {
        exit(0);
    }
    else if(v == SIGCHLD)
    {
#if 0
        // 因为wait是不可靠函数且是个阻塞函数
        // 当有多个子进程死亡，父进行调用这个处理函数，不可靠函数，之保存了一个进程pid，所以只会处理一个尸体
        // 阻塞函数，当子进程一直在处理中没有死亡，父进程会一直阻塞在这里
        wait(NULL);
        printf("signal child");
#endif
        //wait任何版本都是不可靠的
        //所以要循环收尸
        while(1)
        {
            //非阻塞版本
            //第一个参数是需要处理的子进程的pid，-1代表任意进程
            pid_t pid = waitpid(-1,
                    NULL,
                    WNOHANG);
            if(pid == -1)
                break;
            cout << "child signal, pid = " << pid << endl;
        }
        exit(0);
    }
}

int main()
{

    signal(SIGINT, handle);
    signal(SIGCHLD, handle);

    int i;
    for(i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            return 0;
        }
    }

    while(1)
    {
        sleep(1);
    }
    
    return 0;
}
