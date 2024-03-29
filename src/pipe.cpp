#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

//这段代码演示了如何创建一个管道，然后使用两个子进程来运行不同的命令，其中一个子进程将其输出写入管道，而另一个子进程从管道中读取数据，实现了一个类似于Unix命令行管道的效果

int main()
{
    int pipefd[2] = {-1};   //声明了一个名为pipefd的整数数组，用于存储管道的读取端和写入端的文件描述符。初始化为-1是一种安全的做法，表示尚未分配文件描述符
    if (pipe(pipefd) < 0)   //使用pipe系统调用创建一个管道，并将读取端和写入端的文件描述符存储在pipefd数组中。如果管道创建失败，会返回-1
    {
        perror("pipe error");
        return -1;
    }
    //ps程序
    //pid_t 是一个整数类型，通常用于存储进程ID
    //fork() 是一个系统调用，它用于创建一个新的进程，这个新进程是当前进程（称为父进程）的副本。
    //在父子进程之间，大部分资源（例如内存、文件描述符等）是相同的，但它们有不同的进程ID（PID）。父进程和子进程在接下来的执行中可以并行运行。
    //如果 fork() 调用成功，它将返回两次。在父进程中，返回值是子进程的PID，而在子进程中，返回值是0。因此，通过检查 ps_pid 的值，代码可以确定当前是在父进程还是子进程中执行。
    /*
        如果 fork() 返回值为正数（非负整数），那么它在父进程中执行。此时，ps_pid 将包含子进程的PID，可以用于跟踪和管理子进程。
        如果 fork() 返回值为0，那么它在子进程中执行。此时，ps_pid 将为0，表示当前是子进程。
        如果 fork() 失败，它返回-1，表示创建子进程失败。在这种情况下，通常需要处理错误情况。
    */
    pid_t ps_pid = fork();
    if (ps_pid == 0)
    {
    	//将标准输出重定向到管道的写端，将要打印的数据写到管道中,而不是显示在终端上。
        dup2(pipefd[1], 1);
        //将该子进程替换成ps进程
        execlp("ps", "ps", "-ef", NULL);  //使用execlp函数来替换子进程的当前映像，将其替换为ps -ef命令。这样，子进程将运行ps -ef命令并将输出写入管道。
        /*
            execlp 是一个用于执行其他程序的函数。它用于在当前进程的上下文中启动另一个程序，替换当前进程的映像。在这个例子中，ps -ef 命令将在当前进程的上下文中执行。
            "ps" 是第一个参数，通常是要执行的程序的名称或路径。在这里，它是 "ps"，表示要执行 ps 命令。
            "ps" 是第二个参数，它通常是执行程序的名称。这个参数在 ps 命令中没有实际作用，只是一个标识符。
            "-ef" 是第三个参数，通常是传递给执行程序的命令行参数。在这里，它是 -ef，表示要使用 ps 命令的 -ef 参数来列出所有进程的详细信息。
            NULL 是参数列表的终结符，表示参数列表的结束。
        */
        exit(0);                         // 子进程完成任务后退出
    }
    //grep程序
    pid_t grep_pid = fork();
    if (grep_pid == 0)
    {
    	//关闭写端，防止读完数据后发生阻塞
        close(pipefd[1]);
        //将标准输入重定向到管道的读端，从0-标准输入读取的数据相当于从管道读取数据,而不是从终端获取输入
        dup2(pipefd[0], 0);
        //将该子进程替换成grep进程
        execlp("grep", "grep", "ssh", NULL);
        exit(0);
    }
    //关闭父进程的读取端
    close(pipefd[0]);
    close(pipefd[1]);
    //这两行代码等待子进程完成执行
    //waitpid函数会等待指定的子进程终止，并且如果子进程出现任何错误，它会将错误信息存储在第二个参数中（这里传入了NULL，表示不关心错误信息）。
    waitpid(ps_pid, NULL, 0);
    waitpid(grep_pid, NULL, 0);
    return 0;
}
