#include "../header.h"
int main()
{
    if(fork() > 0)
    {
        getchar();
    }
}
