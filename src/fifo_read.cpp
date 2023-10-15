#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

//这段代码创建一个FIFO文件，以只读方式打开它，并不断从FIFO中读取数据，然后将读取的数据打印到屏幕上。这是一种进程间通信的方式，其中一个进程将数据写入FIFO，而另一个进程读取数据。

int main()
{
    umask(0);                    // 这一行设置了文件创建屏蔽字为0，允许创建文件时设定任何权限。这通常用于确保后续创建的FIFO文件具有指定的权限。
    //创建管道文件
    int ret = mkfifo("./test.fifo",0664 ); //创建一个FIFO文件名为 test.fifo，权限设置为 0664，表示所有者和组都有读写权限，其他用户只有读权限。
    //如果FIFO文件已经存在，它将返回错误。如果返回小于0且错误码不是 EEXIST（文件已存在），则输出错误信息并返回-1。
    //管道文件不是因为存在而创建失败
    if (ret < 0 && errno != EEXIST)
    {
        perror("mkfifo error");
        return -1;
    }
    //以只读方式获取管道文件的操作句柄,返回一个文件描述符 fd，用于后续的读取操作。如果打开FIFO文件失败，它将输出错误信息并返回-1。
    int fd = open("./test.fifo", O_RDONLY);
    if (fd < 0)
    {
        perror("open fifo error");
        return -1;
    }
    printf("open fifo success\n");
    int i = 0;
    //一直读数据
    while(1)
    {
        char buf[1024] = {0};
        //将从管道读取的文件写到buf中
        int ret = read(fd, buf, 1023); //尝试从FIFO文件中读取数据，将读取的数据存储到 buf 中，最多读取1023字节。read 函数的返回值 ret 表示实际读取的字节数。
        /*
            如果 ret 小于0，表示读取出错，会输出错误信息并返回-1。
            如果 ret 等于0，表示所有写入FIFO的操作已经关闭（没有更多数据可读），会输出错误信息并返回-1。
            如果 ret 大于0，表示成功读取了数据，代码将打印所读到的数据以及读取的次数 i
        */
        ++i;
        if (ret < 0)
        {
            perror("read error");
            return -1;
        }
        //所有写端都关闭
        else if (ret == 0)
        {
            perror("all write closed");
            return -1;
        }
        //打印所读到的数据
        printf("read buf:[%s] %d\n",buf, i);
    }
    //关闭FIFO文件的文件描述符，释放资源。
    close(fd);
    return 0;
}
