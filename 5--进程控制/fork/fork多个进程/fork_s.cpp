#include "../header.h"

//创建多个进程，如果同时让父进程 和 子进程 都产生进程
//进程数量是以指数增长的
//所以创建奇数个进程，只让父进程调用fork，产生子进程直接返回

int main()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        pid_t pid = fork();
        if(pid == 0)
        {   
            break;
        }
    }
    cout << "hello fork" << endl;
    return 0;
}
