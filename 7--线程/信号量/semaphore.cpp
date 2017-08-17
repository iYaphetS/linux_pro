#include "../header.h"
#include <list>
#include <semaphore.h>
//任务结构体
typedef struct Task
{
    int a;
}Task;

//任务队列
list<Task *> tasks;

//创建锁
pthread_mutex_t mutex;

//定义一个信号量
sem_t sem;

//创建一个自动锁
class AutoLock
{
public:
    AutoLock(pthread_mutex_t &mutex) : mutex(mutex)
    {
        pthread_mutex_lock(&mutex);
    }
    ~AutoLock()
    {
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_t &mutex;
};

void *thread_send(void *ptr)
{
    int i = 0;
    while(1)
    {
        usleep(10 * 1000);
        Task *t = new Task;
        t->a = i++;

        {
            AutoLock lock(mutex);
            tasks.push_back(t);

        }
        sem_post(&sem);

    }
}

void *thread_recv(void *ptr)
{
    while(1)
    {
        //等待信号量，如果信号量为0,阻塞
        sem_wait()&sem;
        pthread_mutex_unlock(&mutex);
        Task *t;
        {
            AutoLock lock(mutex);
            if(tasks.size() == 0)
            {
                continue;
            }
            t =  tasks.front();
            //t =  *tasks.begin();
            tasks.pop_front();
        }
        cout << "t->task:" << t->a << endl;;
        delete t;
    }
}
int main()
{
    //初始化信号量
    sem_init(&sem, 0, 0);


    //设置循环锁
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    //锁的初初始化
    pthread_mutex_init(&mutex, &attr);

    //创建两个线程，一个发任务的线程，一个收任务的线程
    pthread_t tid_s, tid_r;
    pthread_create(&tid_s, NULL, thread_send, NULL);
    pthread_create(&tid_r, NULL, thread_recv, NULL);

    pthread_join(tid_r, NULL);
    pthread_join(tid_s, NULL);
    return 0;
}
