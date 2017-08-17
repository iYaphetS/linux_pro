#include "../header.h"
#include <signal.h>


int main()
{
    //FILE * fp = popen("ifconfig | grep inet | grep -v inet6 | awk '{print $2}' | awk -F \":\" '{print $s1}'", "r");
    FILE * fp = popen("ifconfig | grep inet | grep -v inet6 | awk '{print $2}'", "r");

    char buf[1024];
    while(fgets(buf, sizeof(buf), fp))
    {
        cout << buf;
    }
    pclose(fp);

    return 0;
}
