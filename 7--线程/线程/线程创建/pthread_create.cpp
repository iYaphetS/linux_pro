#include "../header.h"


void *thread_func(void *ptr)
{
    while(1)
    {
        usleep(1000 * 2);
        cout << "in thread" << endl;
    }
}
int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);
    
    while(1)
    {
        usleep(1000 * 1);
        cout << "in process" << endl;
    }

    return 0;
}
