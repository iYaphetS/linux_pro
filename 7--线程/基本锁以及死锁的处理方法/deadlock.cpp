#include "../header.h"

//死锁----1.重复加一把锁2.忘记解锁
//处理方法
//1.循环锁---处理重复加锁的问题
//原理：重复加锁，不会阻塞，它会加一个计数器，解锁时，直到计数器为0, 完成解锁
//2.自动锁---处理忘记解锁
//原理：在锁的析构函数中添加解锁，锁的对象被释放后自动调用了解锁

int a = 0;
int b = 0;
pthread_mutex_t mutex;

class AutoLock
{
public:
    AutoLock(pthread_mutex_t &m) : mutex(m)
    {
        pthread_mutex_lock(&mutex);
    }

    ~AutoLock()
    {
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_t &mutex;

};
void * thread_func(void *ptr)
{
    for(int i = 0; i < 1000; i++)
    {
        AutoLock lock(mutex);
        if(a > 1000)
        {
            return NULL;
        }
        a++;
    }
}

//通过进程中的不同线程是不能操作全局变量
int main()
{
    //设置循环锁的属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    //初始化锁
    pthread_mutex_init(&mutex, &attr);


    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    
    for(int i = 0; i < 1000; i++)
    {
        //用一个大括号{}来表示所的范围，有些不需要加锁的就不需要，比如b，只有这个线程操作它就不需要加锁
        {
            AutoLock lock(mutex);
            a++;
        }
        b++;
    }

    pthread_join(tid, NULL);
    cout << a << endl;
    cout << b << endl;
    return 0; 
}
