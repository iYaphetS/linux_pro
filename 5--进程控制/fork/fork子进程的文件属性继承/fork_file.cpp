#include "../header.h"

//调用fork产生的子进程，会完全继承父进程的文件描述符
//也就是说，无论是子进程还是父进程打开某一文件，子和父进程都会指向同一个文件结构体

int main()
{
    //打开一个文件，文件不存在就创建，读写执行权限
    //0777 ^ 0002-------111 111 111 ^ 000 000 010-------111 111 101-----0775
    int fd = open("f.txt", O_WRONLY|O_CREAT, 0777);

    pid_t pid = fork();

    write(fd, "hello fork\n", 11);

    //在子进程修改文件的属性，父进程会自动拥有这些属性
    if(pid == 0)
    {
        //得到文件读写方式
        int flag = fcntl(fd, F_GETFL);
        flag |= O_APPEND;
        //设置读写方式
        fcntl(fd, F_SETFL, flag);
    }
    else
    {
        sleep(1);//确保子进程属性设置成功

        int flag = fcntl(fd, F_GETFL);
        if(flag & O_APPEND)
        {
            printf("父进程有追加写的属性\n");
        }
        else
        {
            printf("父进程没有追加写的属性\n");

        }
    }
    


    return 0;
}
