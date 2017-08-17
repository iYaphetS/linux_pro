#include "../header.h"
int main()
{
    // printf(); // write(1, ...);
    int ret = write(3, "wirte xxx", 9);
    if(ret < 0)
    {
        printf("error write file\n");
    }
}
