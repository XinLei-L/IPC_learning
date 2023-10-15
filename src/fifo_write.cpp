#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int main()
{
    umask(0);
    int ret = mkfifo("./test.fifo", 0664);
    if (ret < 0 && errno != EEXIST)
    {
        perror("mkfifo error");
        return -1;
    }
    //以只写的方式打开管道文件，返回一个文件描述符 fd，用于后续的写入操作。如果打开FIFO文件失败，它将输出错误信息并返回-1。
    int fd = open("./test.fifo", O_WRONLY);
    if (fd < 0)
    {
        perror("open fifo error");
        return -1;
    }
    printf("open fifo success\n");
    int i = 0;
    //一直写
    while(1)
    {
        char buf[1024] = {0};
        //将字符串内容写到buf中，并记录写的次数
        sprintf(buf, "hello WhiteShirtI", i++);
        //将buf中的内容写到管道中
        write(fd, buf, strlen(buf));
        printf("write data success\n");
        //这一行代码使程序休眠1秒，以控制写入数据的速度，以免太快写入大量数据
        sleep(1);
    }
    close(fd);
    return 0;
}
