
#include "../header.h"

int main()
{

    pid_t pid = fork();
    if(pid == 0)
    {
        // 子进程中执行的代码
        int fd = open("a.txt", O_WRONLY);
        execlp("./writefile", "./writefile", NULL);
        printf("error start program\n");
        return 0;
    }

    wait(NULL); // 阻塞调用
    printf("child program end\n");

}
