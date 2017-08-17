#include "../header.h"


int main()
{
    pid_t pid = fork();

    if(pid == 0)
    {
        setpgid(getpid(), getpid());
        pid_t pid = fork();
        if(pid == 0)
        {
            cout << "孙子进程组id：" << getpgid(getpid()) << endl;
        }
        else
        {
        cout << "父亲进程组id：" << getpgid(getpid()) << endl;
        }
    }
    else
    {
        cout << "爷爷进程组id：" << getpgid(getpid()) << endl;
    }
   //getchar();
    return 0;
}
