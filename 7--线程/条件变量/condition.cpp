#include "../header.h"
#include <list>
//任务结构体
typedef struct Task
{
    int a;
}Task;

//任务队列
list<Task *> tasks;

//创建锁
pthread_mutex_t mutex;

//定义一个条件变量
pthread_cond_t cond;

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

            //唤醒调用了pthread_cond_waitd的线程
            //注意只能唤醒正在等待的线程
            pthread_cond_signal(&cond);
        }

    }
}

void *thread_recv(void *ptr)
{
    while(1)
    {
        //usleep(5 * 1000);//不专业
        //等待条件变量，一定需要一把锁来进行配合
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        /*内部伪代码
        pthread_cond_unlock(&mutex);
        wait.......
        pthread_mutex_lock(&mutex);
          */
        pthread_mutex_unlock(&mutex);
        Task *t;

        //当条件满足时，任务应该全部处理
        while(1)
        {
            AutoLock lock(mutex);
            if(tasks.size() == 0)
            {
                //continue;
                break;
            }
            t =  tasks.front();
            //t =  *tasks.begin();
            tasks.pop_front();
            cout << "t->task:" << t->a << endl;;
            delete t;
        }
    }
}
int main()
{
    //初始化条件变量
    pthread_cond_init(&cond, NULL);


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
