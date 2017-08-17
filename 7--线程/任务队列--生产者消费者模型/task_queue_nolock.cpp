#include "../header.h"
#include <sys/socket.h>
#include <list>
//任务结构体
typedef struct Task
{
    int a;
}Task;

//任务队列
list<Task *> *tasks_send;
list<Task *> *tasks_recv;

//创建两个单向管道
//主线程接受任务的线程recv，给发任务的线程send传递消息
int fd[2];
//发任务的线程send给接任务的线程recv传递消息
int pip[2];

void *thread_send(void *ptr)
{
    int i = 0;
    int needSwap = 0;
    while(1)
    {
        char buf;

        read(fd[0], &buf, 1);

        //主线程通知可以发送任务了,将任务发送到队列
        if(buf == 1)
        {
            Task *t = new Task;
            t->a = i++;
            tasks_send->push_back(t);

            //查看接受线程是否有需要任务的请求
            //如果有，主动将任务交给请求着
            if(needSwap)
            {
                //交换队列
                list<Task *>* temp = tasks_send;
                tasks_send = tasks_recv;
                tasks_recv = temp;
                //通知接受线程有数据
                write(pip[1], &buf, 1);
                needSwap = 0;
            }

        }
        //接受线程通知任务处理完了，请求发送任务
        else if(buf == 2)
        {
            if(tasks_send->size() > 0)
            {
                //交换队列
                list<Task *>* temp = tasks_send;
                tasks_send = tasks_recv;
                tasks_recv = temp;
                //通知接受线程有数据
                write(pip[1], &buf, 1);
                needSwap = 0;
            }
            else
            {
                needSwap = 1;
            }
        }
    }
}

void *thread_recv(void *ptr)
{
    while(1)
    {
        Task *t;
        if(tasks_recv->size() == 0)
        {
            char buf = 2;
            write(fd[1], &buf, 1);
            //这是个阻塞函数，读到内容size就不为0
            //就不用continue
            read(pip[0], &buf, 1);
            //continue;
        }
        t =  tasks_recv->front();
        //t =  *tasks.begin();
        tasks_recv->pop_front();
        cout << "t->task:" << t->a << endl;;
        delete t;
    }
}
int main()
{
    tasks_send = new list<Task *>;
    tasks_recv = new list<Task *>;

    //单向管道
    pipe(pip);
    //全双工管道
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    //创建两个线程，一个发任务的线程，一个收任务的线程
    pthread_t tid_s, tid_r;
    pthread_create(&tid_s, NULL, thread_send, NULL);
    pthread_create(&tid_s, NULL, thread_recv, NULL);

    while(1)
    {
        char buf = 1;
        //从终端读内容到缓冲区，getchar从缓冲区拿字符
        //有多少个字符就会循环多少次，就会向管道写入1多少次
        getchar();
        write(fd[1], &buf, 1);
    }
    pthread_join(tid_r, NULL);
    pthread_join(tid_s, NULL);
    return 0;
}
