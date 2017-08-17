#include "../header.h"
#include <vector>
#include <algorithm>


bool mycompare(const char *s1, const char *s2)
{
    return strcmp(s1, s2) < 0 ? true : false;
}
int main()
{
    //打开当前目录
    DIR *dir = opendir(".");

    vector<char *> v;
    while(1)
    {
        //读目录
        struct dirent *entry = readdir(dir);

        if(entry == NULL)
        {
            break;
        }
        if(entry->d_name[0] == '.')
        {
            continue;
        }
        v.push_back(entry->d_name);

    }

    //sort(v.begin(), v.end(), greater<char *>());
    sort(v.begin(), v.end(), mycompare);
    for(auto it : v)
    {
        cout << it << "  ";
    }
    cout << endl;

    return 0;
}
