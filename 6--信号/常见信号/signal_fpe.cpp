#include "../header.h"
#include <signal.h>
void sig_handle(int v)
{
    cout << "signal_spe" << endl;
    exit(-1);
}
int main()
{
    //SIGFPE---浮点异常，
    //信号能捕获，不能忽略
    //信号会一直发送，直到进程退出
    signal(SIGFPE, sig_handle);
    //signal(SIGFPE, SIG_IGN);
    //
    //GIGKILL----进程停止
    //不能捕获，不能忽略
    //signal(SIGKILL, SIG_IGN);
    //signal(SIGKILL, sig_handle);
#if 1
    int a = 0;
    int b = 10;
    int c = b / a;
#endif
    getchar();
    return 0;
}
