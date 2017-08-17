#include "../header.h"

void* thread_func(void* ptr)
{

}

// 创建分离线程的两种方式
int main()
{
    pthread_t tid;
    pthread_attr_t attr; // 属性中设置分离属性
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, thread_func, NULL);

    pthread_getattr_np(tid, &attr);

    size_t gsize;
    pthread_attr_getguardsize(&attr, &gsize);
    printf("gsize is %d\n", (int)gsize);

    struct sched_param param;
    pthread_attr_getschedparam(&attr, &param);
    printf("priority...=%d\n", (int)param.__sched_priority);

    int policy;
    pthread_attr_getschedpolicy(&attr, &policy);
    printf("policy=%d\n", policy);

    int scope;
    pthread_attr_getscope(&attr, &scope);
    if(scope == PTHREAD_SCOPE_SYSTEM) printf("system\n");
    printf("scope=%d\n", scope);

    void* ptr;
    size_t size;
    pthread_attr_getstack(&attr, &ptr, &size); // 取得stack首地址
    printf("stackaddr=%p, size=%d\n", ptr, (int)size);

    int ret = pthread_join(tid, NULL);
    printf("ret is %d\n", ret);
}

#if 0
int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    // 设置了Detach属性之后，tid指示的线程，在结束之后，自己回收PCB
    pthread_detach(tid);  

    int ret = pthread_join(tid, NULL);
    printf("ret is %d\n", ret);
}
#endif
