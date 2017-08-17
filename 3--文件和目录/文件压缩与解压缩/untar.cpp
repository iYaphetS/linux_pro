
#include "header.h"


int main(int argc, char *argv[])
{
    char *path = argv[1];

    FILE *fpin = fopen(path, "r");

    char buf[4096];

    while(1)
    {
        if(fgets(buf, sizeof(buf), fpin) == NULL)
            break;
        if(strcmp(buf, "f\n") == 0)
        {
            fgets(buf, sizeof(buf), fpin);
            buf[strlen(buf) - 1] = 0;
            //cout << buf << endl;
            //创建一个文件
            FILE *fpout = fopen(buf, "w");

            fgets(buf, sizeof(buf), fpin);
            buf[strlen(buf) - 1] = 0;
            int len = atoi(buf);
            //cout << "len = " << len << endl;

            //读取文件内容写入到创建的那个新文件中
            while(1)
            {
                //处理粘包问题
                int readlen = len > sizeof(buf) ? sizeof(buf) : len;
                //cout << "readlen = " << readlen << endl;
                int ret = fread(buf, 1, readlen, fpin);
                if(ret <= 0)
                {
                    cout << "文件格式有问题" << endl;
                    exit(1);
                }

                fwrite(buf, ret, 1, fpout);

                len -= ret;
                //cout << "len = " << len << endl;
                if(len <= 0)
                    break;
            }

            fclose(fpout);
        }
        else if(strcmp(buf, "d\n") == 0)
        {
            fgets(buf, sizeof(buf), fpin);
            buf[strlen(buf) - 1] = 0;
            //创建一个目录
            mkdir(buf, 0777);
        }
        else if(strcmp(buf, "h\n") == 0)
        {
            //得到需要链接的文件名
            fgets(buf, sizeof(buf), fpin);
            buf[strlen(buf) - 1] = 0;
            string link_file(buf);
            
            //得到被链接的文件名
            //被链接的文件已经解包
            fgets(buf, sizeof(buf), fpin);
            buf[strlen(buf) - 1] = 0;
            string linked_file(buf);

            //int link(const char *oldpath, const char *newpath);
            //如同 ln (存在的文件名) (新建的链接文件名)
            //（新建的链接文件）会自动创建
            //会自动创建这个链接文件
            link(linked_file.c_str(), link_file.c_str());

        }

    }

    return 0;
}
