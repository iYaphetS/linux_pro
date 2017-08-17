#include "cs_net.h"

int main()
{
    //int fd = myClient(9989, "127.0.0.1");
    //这种是长连接
    //当客户端发送一种命令，服务器处理返回后就将它的fd关闭，回到循环开始等待用户连接
    //但是长连接，客户端只与服务其发起链接请求一次，如果还继续给服务器发数据，服务器是不能收到数据的，因为还没有与客户端连接

    char cmd[256];
    char user[256];
    char pass[256];

    while(1)
    {
        int fd = myClient(9990, "127.0.0.1");
        if(fd < 0)
        {
            return -1;
        }
        cout << "please input cmd(r or l):";
        fgets(cmd, sizeof(cmd), stdin);//带\n的字符串
        cmd[strlen(cmd) - 1] = 0;//去掉\n
        if(strcmp(cmd, "r") == 0 || strcmp(cmd, "l") == 0)
        {
            cout << "please input name:";
            fgets(user, sizeof(user), stdin);
            user[strlen(user) - 1] = 0;

            cout << "please input passwd:";
            fgets(pass, sizeof(pass), stdin);
            pass[strlen(pass) - 1] = 0;

            char packet[2048];
            sprintf(packet, "%s|%s|%s", cmd, user, pass);
            myWritebuf(fd, packet);

            char *res = myReadbuf(fd);
            cout << "res:" << res << endl;
            free(res);
        }
        else
        {
            cout << "error command" << endl;
        }

    }

    return 0;
}
