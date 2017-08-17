

#include "../header.h"

pthread_key_t key;

#if 0
struct param
{
    void* p;
    void* p1;
};

void myfree(void* ptr)
{
    struct param* p = (struct param*)ptr;
    free(p->p);
    free(p->p1);
    free(p);
}
#endif

void foo()
{
    void* ptr =  pthread_getspecific(key);
    printf("%s\n", (char*)ptr);
}

void* thread_func(void* ptr)
{
#if 0
    char* p = malloc(sizeof(struct param));
    p->p = malloc(100);
    p->p1 = malloc(200);
#endif
    char* p = malloc(100);
    strcpy(p, "this is sub thread");
    pthread_setspecific(key, p);
    foo();
}

int main()
{
    // 创建key
    //pthread_key_create(&key, myfree);
    pthread_key_create(&key, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    char* p = malloc(100);
    strcpy(p, "this is main thread");
    // 为调用该函数的线程设置私有数据
    pthread_setspecific(key, p);

    foo();

    pthread_exit(0);
  
}
