#include "../header.h"
#include <sys/wait.h>
void print_tips()
{
    char cwd[2048];
    getcwd(cwd, sizeof(cwd));
    printf("myshell:%s > ", cwd);
}

int handle_builtin_cmd(char *comand)
{
    int ret = 0;

    //这个函数相当与malloc一块内存给cmd，将comand内容拷贝给cmd
    char *cmd = strdup(comand);
    char *c = strtok(cmd, " \t");

    if(strcmp(c, "cd") == 0)
    {
        ret = 1;
        char *path = strtok(NULL, " \t"
);
        chdir(path);
    }
    free(cmd);

    return ret;
}

void handle_cmd(char *cmd)
{
    //是否有追加重定向
    int isAppend = 0;
    if(strstr(cmd, ">>"))
    {
        isAppend = 1;
    }

    //有重定向，将命令和文件切割出来
    //没有重定向，cmd1还是cmd
    char *cmd1 = strtok(cmd, ">");
    char *filename = strtok(NULL, ">");
    //去掉文件前面和后面的字符串
    filename = strtok(filename, " \t");
    printf("[%s]\n", filename);

    //得到命令名称
    char *args[250];
    int i = 0;
    char *c = strtok(cmd1, " \t");
    args[i++] = c;

    //得到命令搭配的参数
    //都存放在数组中
    while(1)
    {
        char *arg = strtok(NULL, " \t");
        args[i++] = arg;

        //跳出循环的时机，是最后的哨兵NULL存放到了数组中
        if(arg == NULL)
            break;

    }

    //调用fork和execl函数--让子进程执行另一个程序
    pid_t pid = fork();
    if(pid == 0)
    {
        //是否有重定向
        if(filename)
        {
            int flag;
            //是否是追加重定向
            if(isAppend)
            {
                flag = O_WRONLY|O_APPEND|O_CREAT;
            }
            else
            {
                flag = O_WRONLY|O_TRUNC|O_CREAT;
            }
            int fd = open(filename, flag, 0777);
            //将fd指向了1（终端）--在终端输出的都会输出到fd指向的文件中
            dup2(fd, 1);
            close(fd);
        }
        //int execvp(const char *file, char *const argv[]);
        execvp(c, args);//这个函数调用成功会自动退出，调用不成功会执行后面的代码所以后面代码一定要让这个进程退出，不然进程一直在前台
        exit(0);
    }
    //为子进程清理
    wait(NULL);
}
int main()
{
    while(1)
    {
        print_tips();

        char cmd[1024];

        fgets(cmd, sizeof(cmd), stdin);
        cmd[strlen(cmd) - 1] = 0;

        if(!handle_builtin_cmd(cmd))
        {
            handle_cmd(cmd);
        }
    }

    return 0;
}
