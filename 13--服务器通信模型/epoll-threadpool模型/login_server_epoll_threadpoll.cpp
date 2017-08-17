#include "cs_net.h"
#include <list>
#include <map>

list<int> socks;
pthread_mutex_t mutex;
//pthread_cond_t cond;
sem_t sem;//可以记录信号数量


int epollfd;

struct channel
{
    int fd;
    char *buf;
    int readsize;//readsize = packetsize + 4;
    int packetsize;

    channel(int fd)
    {
        buf = new char[4096];
        readsize = 0;
        packetsize = 0;
        this->fd = fd;
    }
    ~channel()
    {
        close(fd);
        delete buf;
    }
};

map<int, channel*> channels;

void handledata(channel *chan)
{
    int newfd = chan->fd;
    //接受数据
    //格式
    //报文长度    r|username|password-->注册
    //报文长度    l|username|password-->登录
    //char *buf = myReadbuf(newfd);
    char *buf = chan->buf + 4;

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

    delete chan;

}
void recvdata(channel *chan)
{
    int newfd = chan->fd;
    int ret = read(newfd, 
            chan->buf + chan->readsize,
            chan->packetsize - chan->readsize + 4);
    if(ret > 0)
    {
        chan->readsize += ret;
        if(chan->readsize == chan->packetsize + 4)
        {   
            handledata(chan);
        }
    }
    else if((ret < 0 && errno != EAGAIN) || ret == 0)
    {
        pthread_mutex_lock(&mutex);
        //channels.erase(channels.find(newfd));//???
        channels.erase(newfd);//???
        pthread_mutex_unlock(&mutex); 
        delete chan;

    }
    else
    {
        struct epoll_event ev;
        ev.data.fd = newfd;
        ev.events = EPOLLIN|EPOLLONESHOT;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, newfd, &ev);//需要将epollfd设置全局变量

    }
}
void handle(int newfd)
{
    pthread_mutex_lock(&mutex);
    channel *chan = channels[newfd];
    pthread_mutex_unlock(&mutex);

    if(chan->readsize < 4)
    {
        int ret = read(chan->fd, 
                chan->buf + chan->readsize,
                4 - chan->readsize);
        if(ret > 0)
        {
            chan->readsize += ret;
            if(chan->readsize == 4)
            {
                uint32_t *p= (uint32_t*)chan->buf;
                chan->packetsize = ntohl(*p);

                recvdata(chan);
            }
        }
        else if((ret < 0 && errno != EAGAIN)
                || ret == 0)//出错或者对方关闭socket
        {
            pthread_mutex_lock(&mutex);
            //channels.erase(channels.find(newfd));//???
            channels.erase(newfd);//???
            pthread_mutex_unlock(&mutex); 
            delete chan;
        }
        else
        {//继续读
            struct epoll_event ev;
            ev.data.fd = newfd;
            ev.events = EPOLLIN|EPOLLONESHOT;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, newfd, &ev);//需要将epollfd设置全局变量

        }
    }
    else
    {
        recvdata(chan);
    }

#if 0
#endif
}

void *thread_func(void *ptr)
{
    while(1)
    {
        //等待信号，记录数量
        sem_wait(&sem);
        pthread_mutex_lock(&mutex);
        if(socks.size() == 0)
        {
            pthread_mutex_unlock(&mutex);
            break;//一个线程对应一个消息，当有信号却没有任务时，可以设计为线程结束
        }

        int newfd = socks.front();
        socks.pop_front();
        pthread_mutex_unlock(&mutex);

        handle(newfd);
        //常驻信号
#if 0
        struct epoll_event ev;
        ev.data.fd = newfd;
        ev.events = EPOLLIN|EPOLLONESHOT;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, newfd, &ev);
#endif

    }
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
    pthread_mutex_init(&mutex, NULL);
    //pthread_cond_init(&cond, NULL);
    sem_init(&sem, 0, 0);//第二个参数表示非进程间的信号量，第三个参数初始值
    pthread_t tid1, tid2, tid3;//线程池
    pthread_create(&tid1, NULL, thread_func, NULL);
    pthread_create(&tid2, NULL, thread_func, NULL);
    pthread_create(&tid3, NULL, thread_func, NULL);

#if 0
    while(1)
    {
        pthread_mutex_lock(&mutex);
        socks.push_back(newfd);
        pthread_mutex_unlock(&mutex);
        //发送信号，可以处理数据
        pthread_cond_signal(&cond);
    }
#endif

    int epollfd = epoll_create(1024);
    addepoll(epollfd, server, EPOLLIN);

    struct epoll_event ev[8];
    while(1)
    {   
        int ret = epoll_wait(epollfd, ev, 8, 5000);
        if(ret > 0)
        {
            for(int i = 0; i < ret; i++)
            {
                if(ev[i].data.fd == server)
                {
                    int newfd = myAccept(server, NULL, NULL);

                    addepoll(epollfd, newfd, EPOLLIN|EPOLLONESHOT);//EPOLLONESHOT表示通知过的消息将不再通知
                    //myRead函数是阻塞的，它会读满要求的长度，如果没有读满将会阻塞
                    //如果客户端先发送了等下我将一个长度为n字节的数据包的长度信息，但是实际发送数据包的时候，但它发送了一个小于n长度的数据包，且不再发送，那么服务其将一直会阻塞在这里-------（攻击服务器）
                    channel *chan = new channel(newfd);
                    channels[newfd] = chan;

                    int flags = fcntl(newfd, F_GETFL);
                    flags |= O_NONBLOCK;
                    fcntl(newfd, F_SETFL, &flags);
                }
                else
                {
                    pthread_mutex_lock(&mutex);
                    socks.push_back(ev[i].data.fd);
                    pthread_mutex_unlock(&mutex);

                    sem_post(&sem);
                    //pthread_cond_signal(&cond);
                    //epoll_ctl(epollfd, EPOLL_CTL_DEL, ev[i].data.fd, NULL);//由于线程还没有处理完，导致集合里还保留了正在处理的信号，会重复发送，需要取出它， 但这个方法是直接删除，但是可能有些常驻的信号，还需要重新重新发送，就不能直接删除它
                    //我们可以设置信号集合中已经发送过的信号就不再发送，EPOLLONESHOT
                    //如果有些信号需要常驻可以在处理玩这个信号，有重新加入到集合
                }
            }
        }
    }
#if 0

#endif
    return 0;
}
