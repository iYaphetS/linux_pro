#include "../header.h"

/*
 *l (list)          命令行参数列表
 p (path)           搜素file时使用path变量
 v (vector)         使用命令行参数数组
 e (environment)    使用环境变量数组,不使用进程原有的环境变量，设置新加载程序运行的环境变量
 * */
int main()
{
    //int execl(const char *path, const char *arg, ...);
    //int execlp(const char *file, const char *arg, ...);
    
    //execl调用ls就必须要写ls的路径名
    //"/bin/ls"   "/usr/bin/ls"
    //但是execlp直接写ls文件名就可以，他会自动到PATH中去找ls
    execlp("ls", "ls", NULL);
    
    //当execlp调用失败后才会执行后面的代码
    cout << "ls call fail" << endl;

    return 0;
}
