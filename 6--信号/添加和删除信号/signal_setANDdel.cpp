#include "../header.h"
#include <signal.h>
//信号在pcb中存储是用一个32位的整数保存
//每个位与信号的编号对应
//1代表这位上信号触发
//0代表这位上的信号没有触发
//uint32_t sig_set = 0;
int main()
{
#if 0
    //添加了两个信号
    sig_set |= 1 << SIGFPE;
    sig_set |= 1 << SIGUSR1;
    //删除一个信号
    sig_set &= ~(1 << SIGFPE);
#endif
    sigset_t sigset;
#if 0
    //清空所有的信号
    sigemptyset(&sigset);
    //添加了所有的信号
    sigfillset(&sigset);
#endif
    //添加信号
    sigaddset(&sigset, SIGFPE);
    sigaddset(&sigset, SIGUSR1);
    if(sigismember(&sigset, SIGFPE))
    {
        cout << "SIGFPE in member" << endl;
    }
    //删除信号
    sigdelset(&sigset, SIGFPE);
    if(!sigismember(&sigset, SIGFPE))
    {
        cout << "SIGFPE not in member" << endl;
    }

    return 0;
}
