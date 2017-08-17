#include "../header.h"

int a = 10;
pthread_t tid1;
pthread_t tid2;

void *thread_func2(void *ptr)
{

    pthread_join(tid1, NULL);
    a = 200;
    cout << a << endl;
}

void *thread_func1(void *ptr)
{
    //sleep(3);
    pthread_create(&tid2, NULL, thread_func2, NULL);
    cout << a << endl;
}
//线程之间没有父子关系
//同一个进程中的多个线程的全局变量共享
int main()
{
    pthread_create(&tid1, NULL, thread_func1, NULL);

    //各个线程都可以去清理其他线程的尸体
    //清理函数不同与进程的清理函数可以，wait(NULL00)
    //线程必须要知道线程的id
    sleep(1);
    pthread_join(tid2, NULL);
    a = 100;
    cout << a << endl;
    return 0;
}
