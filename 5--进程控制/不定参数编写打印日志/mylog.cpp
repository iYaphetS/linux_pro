#include "../header.h"
#include <stdarg.h>
void __mylog(const char *filename, int line, const char *fmt, ...)
{
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);//ap指向了fmt后面的位置
    vsprintf(buf, fmt, ap);//这个函数会将...所有参数按照fmt格式输出到buf中
    va_end(ap);

    printf("[%s---%d]:%s\n", filename, line, buf);
}

#define mylog(fmt, ...) __mylog(__FILE__, __LINE__, ##__VA_ARGS__)

int main()
{
    int fd = open("1.txt", O_WRONLY);
    if(fd < 0)
    {
        mylog("%s", "写文件失败");
    }
}
