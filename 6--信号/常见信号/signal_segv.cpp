#include "../header.h"
#include <signal.h>
void handle(int v)
{
    cout << "signal_segv" << endl;
    exit(0);
}
int main()
{
    //SIGSEGV-类是与浮点错误，非法访问内存
    //不能被忽略，可以被捕获，会一直捕获，直到进程停止
    signal(SIGSEGV, handle);

    int *p = NULL;

    *p = 10;
#if 0
    c++中的捕获异常就是利用这种原理的
    try
    {
        可能有异常的代码
    }
    catch(返回的异常变量)
    {
        处理异常
    }
#endif
    return 0;
}
