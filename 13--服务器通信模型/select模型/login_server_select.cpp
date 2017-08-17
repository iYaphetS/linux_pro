#include "cs_net.h"

fd_set saveset;//全局set，保存所有存入的文件描述符，不会因为selet函数，去掉被选择的描述符
int maxfd;

void handle(int newfd)
{
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
    FD_CLR(newfd, &saveset);
    if(maxfd = newfd) maxfd--;


}
int main()
{
    int server = myServer(9990, "0.0.0.0");
    //创建文件
    close(open("user.data", O_CREAT|O_EXCL, 0777));
    FD_ZERO(&saveset);//初始化saveset
    FD_SET(server, &saveset);//存入server文件描述符
    maxfd = server;
    while(1)
    {
        fd_set set;//局部set，经过select函数后，set中描述符会减掉，所以每次都需要拷贝全局的set，保证每次循环调用myAccept函数都有server这描述符
        memcpy(&set, &saveset, sizeof(saveset));

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(maxfd + 1, &set, NULL, NULL, &tv);
        if(ret > 0)//有消息
        {
            if(FD_ISSET(server, &set))
            {
                int newfd = myAccept(server, NULL, NULL);
                if(maxfd < newfd)
                {
                    maxfd = newfd;
                }
                FD_SET(newfd, &saveset);
            }
            int i;
            for(i = server+1; i <= maxfd; i++)
            {
                if(FD_ISSET(i, &set))
                {
                    handle(i);
                }
            }
        }
        else if(ret == 0)//没有消息
        {
            continue;
        }
        else if(ret < 0 && errno == EINTR)//被中断打断
        {
            continue;
        }
        else//出错
        {
            cout << "fatal error" << endl;
            break;
        }
    }
#if 0

#endif
    return 0;
}
