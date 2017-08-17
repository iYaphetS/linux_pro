
int main()
{
    int a = 100;
    // 其实在栈上申请100字节
    void* ptr = alloca(a);
    char buf[a];

}
