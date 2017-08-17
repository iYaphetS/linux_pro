#include "../header.h"
#include <signal.h>


void sig_handle(int v)
{
    cout << "signal usr1" << endl;
}
int main1()
{
    signal(SIGUSR1, sig_handle);
    //因为这个注册函数是调用系统函数sigaction实现的，他的flags默认是有SA_RESTART属性，就是阻塞被打断会自动继续调用阻塞函数
    
    //在命令行调用SIGUSR1信号
    //不会打断这个阻塞调用
    //阻塞调用
    getchar();    
    cout << "inteript" << endl;
    return 0;
}

void sig_action(int v, siginfo_t *p, void *ptr)
{
    cout << "signal usr1" << endl;
}
//打断阻塞
int main()
{
    struct sigaction action;
    action.sa_handler = NULL;
    action.sa_sigaction = sig_action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    //action.sa_flags = SA_RESTART;
    action.sa_restorer = NULL;

    sigaction(SIGUSR1, &action, NULL);

    //getchar();
    int fd = open("/dev/input/mice", O_RDONLY);
    if(fd < 0)
    {
        cout << "open error" << endl;
        return -1;
    }

    char buf[10];
    cout << "begin" << endl;
    int ret;
    //阻塞调用
    ret = read(fd, buf, sizeof(buf));
    if(ret == -1)
    {
        //当read返回-1时，判断错误码是不是为EINTR，如果是，说明不是文件读失败而是被别的信号打断的
        if(errno = EINTR)
        {
            cout << "error is EINTR" << endl;
            perror("read");
        }
    }
    cout << "end" << endl;

    cout << "interupt" << endl;
    return 0;
}
