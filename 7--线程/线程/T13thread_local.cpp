

#include "../header.h"
thread_local int a = 50;

void* thread_func(void* ptr)
{
    a = 100;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    sleep(1);
    printf("%d\n", a);
}
