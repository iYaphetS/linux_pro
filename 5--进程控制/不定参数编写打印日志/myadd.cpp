#include "../header.h"
#include <stdarg.h>


int add(int count, int a, int b, ...)
{
    int ret = a + b;
    va_list ap;
    va_start(ap, b);//ap指向了b变量后面
    while(count > 0)
    {
        ret += va_arg(ap, int);//指定了...参数类型为int
        count--;
    }
    va_end(ap);

    return ret;
}

int main()
{
    //第一个参数是除了第二 三个参数外，其他参数的个数
    int sum = add(2, 10, 20, 30 , 40);
    
    cout << sum << endl;
    return 0;
}
