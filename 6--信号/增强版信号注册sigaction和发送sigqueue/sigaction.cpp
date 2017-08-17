#include "../header.h"
#include <signal.h>
void sig_handle(int v)
{
    cout << "signal int" << endl;
}

void sig_action(int v, siginfo_t *info, void *ptr)
{
    cout << "signal int" << endl;
    cout << "siv_int = " << info->si_int << endl;
}
//sigaction----为siganl的增强版，信号注册
//sigqueue----为kill的增强版，信号发送
int main()
{
#if 0
    signal(SIGINT, sig_handle);
    kill(getpid(), SIGINT);
#endif

    struct sigaction action;
    action.sa_handler = NULL;
    action.sa_sigaction = sig_action;
    //上面两个函数是个联合体，只能赋值一个
    sigemptyset(&action.sa_mask);
    //将SIGUSER1设置为掩盖信号，就是在信号处理全局变量时候，不去打断处理函数
    sigaddset(&action.sa_mask, SIGUSR1);
    //action.sa_flags = 0;
    //调用sigqueue传入参数，需要设置flags为SA_SIGINFO
    action.sa_flags = SA_SIGINFO;
    //默认已经废弃，指定为NULL
    action.sa_restorer = NULL;

    /*
     *int sigaction(int signum, 
     const struct sigaction *act,
     struct sigaction *oldact);
     * */
    sigaction(SIGINT, &action, NULL);
    
    union sigval v;
    v.sival_int = 100;
    sigqueue(getpid(), SIGINT, v);
    sleep(1);
    return 0;
}
