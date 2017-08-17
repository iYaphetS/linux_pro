#include "cs_net.h"
#include <list>

//用于接受客户端请求的线程数量（可变）
int accept_threads = 4;
//用于处理数据的线程数量（可变）
int work_threads = 8;

//客户端与服务器通信的数据信息缓冲区
class channel
{
public:
    char *buf;
    uint32_t readsize;//packetsize + 4(4是表示数据包长度的值占4个字节)
    uint32_t packetsize;
    int fd;
    channel(int fd)
    {
        buf = new char[4096];
        readsize = 0;
        packetsize = 0;
        this->fd = fd;
    }
    ~channel()
    {
        delete [] buf;
    }
};


//自动解锁，锁对象被析构时自动解锁
class Autolock
{
public:
    Autolock(pthread_mutex_t &mutex):mutex(mutex)
    {
        //this->mutex = mutex;
        pthread_mutex_lock(&mutex);
    }
    ~Autolock()
    {
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_t &mutex;
};
#define myLock(mutex) Autolock _lock_(mutex)


//线程锁
pthread_mutex_t mutex;
//信号量
sem_t sem;
//任务集合
list<channel *> channels;
//存放空闲的channel对象(对象池)
list<channel *> channels_free;

//当需要channel对象时，不用去new，如果队列中有
//则直接拿过来，没有则去申请
channel *allocchannel(int fd)
{
    channel *ret = NULL;
    myLock(mutex);
    if(channels_free.size() > 0)
    {
        ret = channels_free.front();
        ret->fd = fd;
        channels_free.pop_front();
    }
    else
    {
        ret = new channel(fd);
    }

    return ret;
}
//（本来需delete）即暂时不用的channel对象放到放到线程池队列中
void freechannel(channel *chan)
{
    close(chan->fd);
    chan->readsize = 0;
    chan->packetsize = 0;
    myLock(mutex);
    channels_free.push_back(chan);
}
//服务器的文件描述符
int server;
//信号文件描述符
int epollfd;

//设置信号发送一次就不再发送
void epoll_add(int newfd)
{
    channel *chan  = allocchannel(newfd);//= new channel(newfd);
    struct epoll_event ev;
    ev.data.ptr = chan;//ev_data是个枚举
    ev.events = EPOLLIN|EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, chan->fd, &ev);
}

//设置信号重新放回消息队列
void epoll_mod(channel *chan)
{
    struct epoll_event ev;
    ev.data.ptr = chan;//ev_data是个枚举
    ev.events = EPOLLIN|EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, chan->fd, &ev);

}

//创建的多个线程竞争接受客户端链接，收取数据
void *accept_thread_func(void *ptr)
{
    while(1)
    {
        int newfd = accept(server, NULL, NULL);
        if(newfd > 0)
        {
            //设置非阻塞
            int flags;
            flags = fcntl(newfd, F_GETFL);
            flags |= O_NONBLOCK;
            fcntl(newfd, F_SETFL, &flags);
            epoll_add(newfd);
        }
    }

    return NULL;
}

//读数据包 头部信息
channel *readdata(channel *chan)
{
    int ret;
    if(chan->readsize < 4)
    {
        ret = read(chan->fd,
                chan->buf + chan->readsize,
                4 - chan->readsize);
        if(ret > 0)
        {
            chan->readsize += ret;
            if(chan->readsize == 4)
            {
                uint32_t temp = *(uint32_t *)chan->buf;
                chan->packetsize = ntohl(temp);
            }
            return readdata(chan);//递归, 如果readsize不足4就继续读满4个字节，得到数据包的长度，当readsize已经大于4就会进入另一个分支，去读数据包
        }
    }
    else
    {
        ret = read(chan->fd,
                chan->buf + chan->readsize,
                chan->packetsize + 4 - chan->readsize);
        if(ret > 0)
        {
            chan->readsize += ret;
            if(chan->readsize == chan->packetsize + 4)
            {
                return chan;
            }
            else      
                return readdata(chan);
        }
    }

    //如果对方关闭了socket或者socket有错误
    if(ret == 0 || (ret < 0 && errno != EAGAIN))
    {
        /*
         *从字面上来看，是提示再试一次。这个错误经常出现在当应用程序进行一些非阻塞(non-blocking)操作(对文件或socket)的时候。例如，以 O_NONBLOCK的标志打开文件/socket/FIFO，如果你连续做read操作而没有数据可读。此时程序不会阻塞起来等待数据准备就绪返回，read函数会返回一个错误EAGAIN，提示你的应用程序现在没有数据可读请稍后再试。
         * */
        //delete chan;
        freechannel(chan);//将不用的channel对象放到对象池中
    }
    else//读写出错，就将这个信号继续放到信号集合中，不会因为ONESHOT不再发送这个信号
    {
        epoll_mod(chan);
    }

    return NULL;
}

//处理数据包
void handledata(channel *chan)
{
    //chan->readsize = 0;
    //chan->buf[chan->readsize] = 0;
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
    
    //delete chan;
    freechannel(chan);
}

//多个线程处理数据
void *work_thread_func(void *ptr)
{
    while(1)
    {
        //等待信号（阻塞）
        sem_wait(&sem);
        channel * chan = NULL;
        //从任务集合中拿出一个一个任务处理
        {
            myLock(mutex);
            if(channels.size() == 0)
                break;
            chan = channels.front();
            channels.pop_front();
        }
        //任务能够处理的条件
        if(readdata(chan) != NULL)//当条件成立时chan->readsize == chan->packetsize + 4
        {
            handledata(chan);
        }
    }
    return NULL;
}


void init()
{
#if 1
    //在初始化时，将线程池中添加多余的空闲channnel对象
    //连续申请，能够避免断续申请内存带来的内存碎片化的问题
    for(int i = 0; i < 10000; i++)
    {
        channel *chan = new channel(0);
        channels_free.push_back(chan);
    }
#endif
    //初始化服务器
    server = myServer(10000, "0.0.0.0");
    //信号集合
    epollfd = epoll_create(1024);
    /*
     *该函数生成一个epoll专用的文件描述符。它其实是在内核申请一空间，用来存放你想关注的socket fd上是否发生以及发生了什么事件。size就是你在这个epoll fd上能关注的最大socket fd数
     * */


    pthread_t tid;
    //创建一堆用于接受客户端请求和消息的线程
    for(int i = 0; i < accept_threads; i++)
    {
        pthread_create(&tid, NULL, accept_thread_func, NULL);
    }

    //创建一堆用于处理消息的进程
    for(int i = 0; i < work_threads; i++)
    {
        pthread_create(&tid, NULL, work_thread_func, NULL);
    }

    //循环锁,避免重复加锁阻塞，但多次加锁就要解锁多次
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    //锁的初始化
    pthread_mutex_init(&mutex, &attr);

    //信号量的初始化
    sem_init(&sem, 0, 0);
}

void exec()
{
    struct epoll_event ev[8];
    while(1)
    {
        /*
         * 等侍注册在epfd上的socket fd的事件的发生，如果发生则将发生的sokct fd和事件类型放入到events数组中。
         * */
        int ret = epoll_wait(epollfd, ev, 8, 5000);
        if(ret > 0)
        {
            for(int i = 0; i < ret; i++)
            {
                channel *chan = (channel *)ev[i].data.ptr;
                {
                    myLock(mutex);
                    channels.push_back(chan);
                }
                //发送信号
                sem_post(&sem);
            }
        }
        else if(ret < 0 && errno != EINTR)
        {
            break;
        }
    }
}

int main()
{
    //初始化服务器
    init();
    //
    exec();

    return 0;
}
