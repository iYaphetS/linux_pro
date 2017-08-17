#include "cs_net.h"



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
    close(newfd);//调用colse会在epoll集合中自动去掉这个文件描述符

}

void addepoll(int epollfd, int serverfd, int event)
{
    struct epoll_event ev;
    ev.data.fd = serverfd;
    ev.events = event;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, serverfd, &ev);

}
int main()
{
    int server = myServer(9990, "0.0.0.0");
    //创建文件
    close(open("user.data", O_CREAT|O_EXCL, 0777));
    int epollfd = epoll_create(1024);
    addepoll(epollfd, server, EPOLLIN);

    struct epoll_event ev[8];
    while(1)
    {
        //epoll也不需要每次都要初始化，来保证集合中每次循环都有server，他会保存没有调用colse(fd)的所有文件描述符
        int ret = epoll_wait(epollfd, ev, sizeof(ev)/sizeof(*ev), 5000);
        if(ret > 0)
        {
            int i;
            for(i = 0; i < ret; i++)
            {
                if(ev[i].data.fd == server)
                {
                    int newfd = myAccept(server, NULL, NULL);
                    addepoll(epollfd, newfd, EPOLLIN);
                }
                else
                {
                    handle(ev[i].data.fd);
                }
            }
        }
        else if(ret == 0)//没有消息
        {
            continue;
        }
        else if(ret < 0 && errno == EINTR)//打断
        {
            continue;
        }
        else
        {
            cout << "fatal" << endl;
            break;
        }
    }

#if 0
#endif 

    return 0;
}
