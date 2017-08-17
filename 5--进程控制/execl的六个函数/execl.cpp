#include "../header.h"

/*
 *      文件描述符标志（当前只定义了一个文件描述符标志：FD_CLOEXEC）：仅仅是一个标志，当进程fork一个子进程的时候，在子进程中调用了exec函数时就用到了该标志。意义是执行exec前是否要关闭这个文件描述符；
        文件状态标志：是在系统文件表中，关于write、read等标志
 *
 *      当打开一个文件时，默认情况下文件句柄在子进程中也处于打开状态。因此sys_open()中要复位对应比特位。
 *      设置close on exec标志可通过fcntl(fd, F_SETFD, flags | FD_CLOEXEC)来进行设置，当然flags可先通过F_GETFD获得。
 * */
int main()
{
    //替换后的程序会继承以前的文件描述符,当设置了文件描述标志，就可关掉这个文件描述符
    int fd = open("a.txt", O_WRONLY);

    int flag = fcntl(fd, F_GETFD);
    fcntl(fd, F_SETFD, FD_CLOEXEC | flag);

    //这样设置也可以
    //int fd = open("a.txt", O_WRONLY | FD_CLOEXEC);
    //在一个程序里执行另一个程序
    //int execl(const char *path, const char  *arg, ...);
    //path-----需要执行的程序路径
    //arg 和 ... -------需要执行的程序的命令行参数最后一定要以NULL结束，NULL表示结束标志
    //int ret = execl("./hello", "./hello", NULL);
    int ret = execl("./writefile", "./writefile", NULL);

    //在调用execl函数成功返回后，后面这些代码都不会执行
    //这个程序代码会被完全替换为需要执行的那个程序
    //但这个程序的PCB还是原来的那个PCB
    //
    //当execl执行失败，就会执行这里的代码
    if(ret == 0)
    {
        cout << "success" << endl;
    }
    else
    {
        cout << "fail" << endl;
    }

    return 0;
}
