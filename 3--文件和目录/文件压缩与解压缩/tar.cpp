#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include <map>

//硬链接只能链接文件，且只能在一个目录下
//而软链接不同
map<ino_t, string> fileinfo;
void tar_file(string src, FILE *fpout)
{
    struct stat stat_buf;
    stat(src.c_str(), &stat_buf);

    //检查这个文件是否已经保存，是否是其他文件的硬链接
    //如果这个编号的文件已经在容器中保存过，字符串filename就不是空字符串，他就有这个文件名字
    //也可通过multimap<ino_t, string> fileinfo;
    //if(count(stat_buf.st_ino) > 1)---也可说明是硬链接
    //因为一个键值（文件id）对于多个文件
    string filename = fileinfo[stat_buf.st_ino];
    if(filename.size() != 0)
    {
        //标示是硬链接
        fprintf(fpout, "h\n");
        //文件名
        fprintf(fpout, "%s\n", src.c_str());
        //被链接的文件名
        fprintf(fpout, "%s\n", filename.c_str());

        return;
    }

    //标示打包的是文件
    fprintf(fpout, "f\n");
    //打包的文件名
    fprintf(fpout, "%s\n", src.c_str());

    //打包文件大小
    fprintf(fpout, "%d\n", (int)stat_buf.st_size);

    //打开源中文件读取内容写到目标文件中
    FILE *fpin = fopen(src.c_str(), "r");

    char buf[4096];

    while(1)
    {
        int ret = fread(buf, 1, sizeof(buf), fpin);          
        if(ret <= 0)
            break;
        fwrite(buf, ret, 1, fpout);
    }

    fclose(fpin);

    //向map容器中保存文件的信息（文件ID  文件名）
    fileinfo[stat_buf.st_ino] = src;

}

void tar_dir(string src, FILE *fpout)
{
    //标示打包的是目录
    fprintf(fpout, "d\n");
    //打包目录名,加上\n利于解包按行读
    fprintf(fpout, "%s\n", src.c_str());

    DIR *dir = opendir(src.c_str());
    while(1)
    {
        struct dirent *entry = readdir(dir);
        //没有子目录或者文件
        if(entry == NULL) break;
        if(strcmp(entry->d_name, ".") == 0)
            continue;
        if(strcmp(entry->d_name, "..") == 0)
            continue;

        //如果是普通文件
        if(entry->d_type == DT_REG)
        {
            tar_file(src + "/" + entry->d_name, fpout);
        }
        //如果是目录
        else if(entry->d_type == DT_DIR)
        {
            tar_dir(src + "/" + entry->d_name, fpout);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    char cwd[1024];
    //得到当前路径
    getcwd(cwd, sizeof(cwd));
    //得到目标的绝对路径
    string dst = string(cwd) + "/" + argv[1];

    string src = argv[2];
    cout << "_src = " << src << endl;
    //将工作目录切换到src所在的上一级目录
    //例如当前工作目录 /home/wenc/linux/03/
    //src = "../../TEST"
    //绝对路径/home/wenc/TEST
    //这个工作路径一定要切换，不然在读文件的时候在当前目录找要读的文件，就会找不到
    chdir((src + "/..").c_str());
    //切换工作路径后变成  /home/wenc
    getcwd(cwd, sizeof(cwd));
    //将相对路径转化为绝对路径
    realpath(argv[2], cwd);    
    //反搜索得到源文件名
    src = rindex(cwd, '/') + 1;
    //realpath(src.c_str(), cwd);
    //cout << cwd << endl;//  /home/wenc/TEST
    if(argc < 3)
    {   
        cout << "参数不足" << endl;
        return -1;
    }
    FILE *fpout = fopen(dst.c_str(), "w");

    //tar_dir(src, fpout);
#if 1   
    struct stat stat_buf;
    stat(src.c_str(), &stat_buf);
    //cout << stat_buf  << endl;
    //如果是普通文件
    if(S_ISREG(stat_buf.st_mode))
    {
        tar_file(src, fpout);
        cout << "file" << endl;
    }
    //如果是目录
    if(S_ISDIR(stat_buf.st_mode))
    {
        cout << "dir" << endl;
        tar_dir(src, fpout);
    }
#endif

    fclose(fpout);
    return 0;
}
