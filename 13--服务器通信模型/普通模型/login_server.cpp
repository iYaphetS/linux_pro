#include "cs_net.h"

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


    return 0;
}
