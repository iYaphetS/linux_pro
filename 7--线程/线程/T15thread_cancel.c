
#include "../header.h"

void* thread_func(void* ptr)
{
#if 0
    // 设置casncelbility为false
    while(1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        sleep(1);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
#endif

    while(1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        // 它就是一个退出点
        sleep(1);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        // 检查cancel状态
        pthread_testcancel(); // 如果外部有人调用了pthread_cancel本线程，那么此时该线程退出
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    // 调用cancel之后，线程就被干掉了
    pthread_cancel(tid);

    pthread_join(tid, NULL);
    
    printf("exit \n");
}
