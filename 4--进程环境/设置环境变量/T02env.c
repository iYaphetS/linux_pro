#include "../header.h"

// glibc中定义的全局变量，保存所有的环境变量
extern char** environ;

int main()
{
#if 1
    setenv("xxx", "yyy", 1);

    int i;
    for( i=0; ;++i)
    {
        if(environ[i] == NULL)
            break;
        printf("%s\n", environ[i]);
    }
#endif
    getchar();

#if 0
    // 不同的工作目录，对程序结果的影响
    int fd = open("a.txt", O_RDONLY);
    if(fd < 0)
    {
        printf("error open a.txt\n");
    }
#endif
}
