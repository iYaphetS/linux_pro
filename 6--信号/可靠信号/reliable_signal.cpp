#include "../header.h"
#include <signal.h>

void handle_rt_sig(int v)
{
    cout << "reliable signal" << endl;
}

void setmask(sigset_t *set)
{
    sigprocmask(SIG_BLOCK, set, NULL);
}

void unsetmask(sigset_t *set)
{
    sigprocmask(SIG_UNBLOCK, set, NULL);
}

//大于31的信号都是可靠信号
//一个进程收到一个可靠信号没有处理（掩盖），同时有收到多个同样的信号，这几个信号不会像不可靠信号合并为一个信号，他会全部保存起来
int main()
{
    signal(34, handle_rt_sig);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, 34);
    setmask(&mask);

    kill(getpid(), 34);
    kill(getpid(), 34);
    kill(getpid(), 34);
    kill(getpid(), 34);
    
    unsetmask(&mask);

    return 0;
}
