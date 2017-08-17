#include "../header.h"

//ret为进程最后正常退出时的返回值，ptr为on_exit的第二个参数
void func(int ret, void *ptr)
{
    cout << ret << endl;
}

int foo()
{
    exit(100);//程序退出，返回100
}
int on_ex()
{
    //在进程<正常>退出时，会自动调用这个函数
    //int on_exit(
    //void (*function)(int , void *),
    // void *arg);
    // 第一个参数回调函数指针，第二个参数是回调函数的第二个参数
    on_exit(func, NULL);
    return 0;
}

int main()
{
    
    on_ex();//这个函数中调用on_exit函数，但是这个函数不会执行，因为它要等到进程退出
    //foo();
    abort();//异常推出
    return 0;
}
