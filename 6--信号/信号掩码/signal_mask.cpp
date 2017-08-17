#include "../header.h"
#include <signal.h>
//设置信号掩盖
void setmask(sigset_t *set)
{
    sigprocmask(SIG_BLOCK, set, NULL);
}

//取消信号掩盖
void unsetmask(sigset_t *set)
{
    sigprocmask(SIG_UNBLOCK, set, NULL);
}
int main()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    //SIGKILL是不能掩盖的
    //sigaddset(&mask, SIGKILL);

    setmask(&mask);

    int i = 0;
    while(1)
    {
        sleep(1);
        i++;
        if(i > 30)
        {
            unsetmask(&mask);
        }
    }
    return 0;
}
