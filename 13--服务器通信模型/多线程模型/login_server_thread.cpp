#include "cs_net.h"

void *thread_func(void *ptr)
{
    int newfd = (int)(intptr_t)ptr;
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
}


int main()
{
    int server = myServer(9990, "0.0.0.0");
    //创建文件
    close(open("user.data", O_CREAT|O_EXCL, 0777));

    while(1)
    {
        //阻塞等待用户链接
        int newfd = myAccept(server, NULL, NULL);
        if(newfd < 0)
        {
            break;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, thread_func, (void*)(intptr_t)newfd);

        pthread_detach(tid);
        /*
         *在任何一个时间点上，线程是可结合的（joinable）或者是分离的（detached）。一个可结合的线程能够被其他线程收回其资源和杀死。在被其他线程回收之前，它的存储器资源（例如栈）是不释放的。相反，一个分离的线程是不能被其他线程回收或杀死的，它的存储器资源在它终止时由系统自动释放
         * */
    }

#if 0
#endif

    return 0;
}
