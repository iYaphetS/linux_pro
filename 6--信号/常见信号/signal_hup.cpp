#include "../header.h"
#include <signal.h>

void sig_handle(int v)
{

}
int main()
{
    //SIGHUP---当session leader退出时，会给同session的其他进程发出信号,调用处理函数
    signal(SIGHUP, sig_handle);

    //信号处理函数完成后，处理getchar
    //但是终端关闭后，getchar字符设备文件关闭会立即返回-1结束
    //getchar();

    //上面让进程不退出方式不行
    while(1)
    {
        sleep(1);
    }
    return 0;
}


