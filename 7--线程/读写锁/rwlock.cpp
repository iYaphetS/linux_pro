#include "../header.h"

//定义锁
pthread_rwlock_t mutex;

int a = 0;

void *thread_func(void *ptr)
{
    //读锁
    pthread_rwlock_rdlock(&mutex);
    //只能对数据进行读
    if(a == 0)
    {
        cout << "read" << endl;
    }
    pthread_rwlock_unlock(&mutex);

    //写锁
    pthread_rwlock_wrlock(&mutex);
    for(int i = 0; i < 100; i++)
    {
        a++;
    }
    //可以对数据进行读写
    pthread_rwlock_unlock(&mutex);

}

int main()
{
    //锁的初始化
    pthread_rwlock_init(&mutex, NULL);


    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);


    pthread_join(tid, NULL);


    return 0;
}
