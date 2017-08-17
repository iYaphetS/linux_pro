#include "cs_net.h"

void sig_handle(int v)
{
    if(v == SIGCHLD)
    {
        //SIGCHLD是不可靠信号，多个重复信号只能保存一个
        while(1)
        {
            //非阻塞收尸
            //如果有进程退出，返回子进程的pid
            //没有进程退出返回0
            //错误返回-1
            pid_t pid = waitpid(-1, NULL, WNOHANG);
            if(pid <= 0)
            {
                break;
            }
        }
    }
}
int main()
{
    signal(SIGCHLD, sig_handle);
    int server = myServer(9990, "0.0.0.0");
    //创建文件
    close(open("user.data", O_CREAT|O_EXCL, 0777));
    int newfd;
    while(1)
    {
        //阻塞等待用户链接
        newfd = myAccept(server, NULL, NULL);
        if(newfd == -1)
        {
            return 0;//需要返回
        }
        pid_t pid = fork();
        if(pid == 0)
        {
            break;
        }
        close(newfd);//父进程必须关闭父进程的文件描述符
    }

    close(server);
    //------------------------------------
    //子进程
    //接受数据
    //格式
    //报文长度    r|username|password-->注册
    //报文长度    l|username|password-->登录
    char *buf = myReadbuf(newfd);
    char *saveptr;
    const char *type = strtok_r(buf, "|", &saveptr);
    const char *user = strtok_r(NULL, "|", &saveptr);
    const char *pass = strtok_r(NULL, "|", &saveptr);

    if(type[0] == 'r')
    {
        //将注册成功的用户信息存储到文件
        FILE *fp = fopen("user.data", "a+");
        char buf[1024] = {0};
        char ubuf[1024] = {0};
        char pbuf[1024] = {0};
        char *pend;
        while((pend = fgets(buf, sizeof(buf), fp)) != NULL )
        {
            sscanf(buf, "%[^=]=%[^\n]", ubuf, pbuf);
            if(strcmp(ubuf, user) == 0 && strcmp(pbuf, pass) == 0)
            {
                myWritebuf(newfd, "you had registered");
                break;
            }
        }

        if(pend == NULL)
        {
            fprintf(fp, "%s=%s\n", user, pass);
            myWritebuf(newfd, "register success");
        }
        fclose(fp);
    }
    else if(type[0] == 'l')
    {
        char buf[1024] = {0};
        char ubuf[1024] = {0};
        char pbuf[1024] = {0};
        char *pend;//flag
        FILE *fp = fopen("user.data", "r");
        while((pend = fgets(buf, sizeof(buf), fp)) != NULL )
        {
            sscanf(buf, "%[^=]=%[^\n]", ubuf, pbuf);
            if(strcmp(ubuf, user) == 0 && strcmp(pbuf, pass) == 0)
            {
                myWritebuf(newfd, "login success");
                break;
            }
        }
        if(pend == NULL)//遍历完整个文件,没有这个用户
        {
            myWritebuf(newfd, "user or passwd error");
        }
        fclose(fp);
    }
    free(buf);
    close(newfd);



    return 0;
}
