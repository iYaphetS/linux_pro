#include "../header.h"
#include "signal.h"
void handle(int v)
{
    cout << "signal int"<< endl;
}

int main()
{
    signal(SIGINT, handle);

    pid_t pid = fork();

    if(pid == 0)
    {
        //kill(pid_t pid, int sig);
        //pid == 0-----给这个进程所属组的所有进程发送信号
        //pid == -1----给init组的所有进程发信号，但是有权限，只能是此进程的所属用户的所有进程
        //pid值小于-1,比如-100,就是给进程为100的所属组的所有用户发信号
        //当sig == 0,就是检查一下有没有这个进程
        kill(0, SIGINT);
        sleep(1);

        while(1)
        {
            sleep(1);
        }

    }
    else
    {
        while(1)
        {
            sleep(1);
        }
    }

    return 0;
}
