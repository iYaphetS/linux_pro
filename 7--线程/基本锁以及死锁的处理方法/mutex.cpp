#include "../header.h"
#include <list>

pthread_mutex_t mutex;
int a = 0;
int b = 0;

void * thread_func(void *ptr)
{
    for(int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        a++;
        b++;
        pthread_mutex_unlock(&mutex);
    }
}

//通过进程中的不同线程是不能操作全局变量
int main()
{

    pthread_mutex_init(&mutex, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    
    for(int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        b++;
        pthread_mutex_unlock(&mutex);
    }

    pthread_join(tid, NULL);
    cout << a << endl;
    cout << b << endl;
    return 0; 
}
