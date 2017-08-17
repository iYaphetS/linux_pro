#include "../header.h"
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        return 88;//子进程退出
    }
    else
    {
        //sleep(1);
        int status;
        //pid-----收尸的是哪个子进程的pid
        //sattus--通过wait函数可以得到被收尸的哪个子进程的运行结果
        /*
         *status并非单纯的保存了挂掉的子进程的运行结果，还携带了别的信息，只有其中8位保存了运行结果
可以通过WEXITSTATUS(status)得到，但是必须在正常退出情况下才用
         * */
        //这是一个阻塞函数，直到子进程挂掉后才会被执行
        pid_t pid = wait(&status);
        if(WIFEXITED(status))
        {
            cout << "nomal exit" << endl;
            cout << "子进程的返回结果：" << WEXITSTATUS(status) << endl;
        }
        cout << status << endl;
    }

    return 0;
}
