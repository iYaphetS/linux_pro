
int main()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        // 子进程
        //
        // 继续工作
        setsid(); // 首先独立一个session
        setcwd(...);
        chroot(...);

        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 1);
        close(fd);

        while(1)
        {
            //
        }
    }

    // 父进程是一个组长，它不能独立
    return 0;
}
