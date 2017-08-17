
#include <stdio.h>
#include <sys/times.h>

int main()
{
    int i;
    struct tms buf1;
    times(&buf1);

    // 有一万行代码
    for(i=0; i<100000; ++i)
    {
        printf("hello world\n");
    }

    struct tms buf2;
    times(&buf2);


    printf("user is %d, sys is %d\n", 
            (int)(buf2.tms_utime - buf1.tms_utime),
            (int)(buf2.tms_stime - buf1.tms_stime));
}
