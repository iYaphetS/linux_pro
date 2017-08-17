#include "../header.h"
#include <signal.h>
int main1()
{
    int fd[2];
    int ret = pipe(fd);
    
    //测试管道最大尺寸，当缓冲区满了后写数据会阻塞
    int i = 0;
    while(1)
    {
        write(fd[1], "a", 1);
        i++;
        cout << i << endl;
    }
}


/*
 *管道，两端只有一端打开
 *如果是写端关闭，读端将数据读完，返回EOF
 *如果是读端关闭，写端将不能写入数据，会发出一个信号SIGPIPE（管道破裂）
 * */

void sig_handle(int v)
{
    cout << "signal pipe" << endl;
}
int main2()
{
    int fd[2];
    int ret = pipe(fd);
    signal(SIGPIPE, sig_handle);

    close(fd[0]);
    ret = write(fd[1], "ha", 2);
    if(ret < 0)
    {
        perror("write");
    }

    return 0;
}
int main()
{
    //管道两端，fd[0]一端只读，fd[1]一端只写，所以要创建两个文件描述符号
    int fd[2];
    //int fd;//error
    //创建两个管道
    //管道是内核中一段内存，他不像磁盘文件可以重复读，此读一次就把内存中内容拿走
    int ret = pipe(fd);

    pid_t pid = fork();
    //两个进程，每个进程都有两个文件描述符
    if(pid == 0)
    {
        sleep(1);
        close(fd[0]);
        write(fd[1], "hello", 5);
    }
    else
    {
        close(fd[1]);
        char buf[1024] = {0};
        //读管道文件是阻塞的，等到有数据才返回,在子进程中休眠1秒来证明
        read(fd[0], buf, sizeof(buf));
        cout << buf << endl;
    }

    return 0;
}
