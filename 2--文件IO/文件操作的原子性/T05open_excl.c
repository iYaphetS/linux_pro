
int main()
{
    int fd = open("b.tmp", O_CREAT|O_EXCL, 0777);
}

// 假设这个是另外一个程序
int main()
{
    // 一个创建会成功，而另外一个会失败
    // 这个通常可以用于单例
    int fd = open("b.tmp", O_CREAT|O_EXCL, 0777);
}
