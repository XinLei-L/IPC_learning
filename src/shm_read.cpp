#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

//这段代码创建了一个共享内存区域，将其映射到当前进程，然后在一个无限循环中不断读取并打印共享内存中的数据。这提供了一种进程间通信的方式，多个进程可以共享同一块内存来交换数据

//自定义标识符，最终是以16进制来显示,用于标识共享内存区域。IPC键是一个唯一的整数，通常用于创建和访问共享内存
#define IPC_KEY 0x12345678
int main()
{
    //1.创建共享内存,如果 shmget 调用失败，它会返回-1，并输出错误信息
    int shm_id = shmget(IPC_KEY, 32, IPC_CREAT|0664);
    /*
        IPC_KEY：用于标识共享内存的IPC键。
        32：共享内存的大小，以字节为单位。在这里，共享内存的大小是32字节。
        IPC_CREAT | 0664：指定创建标志和权限。IPC_CREAT 表示如果共享内存不存在，则创建它。0664 权限表示创建后的共享内存可以被创建者及其所属组读写。
    */
    if (shm_id < 0) {
        perror("shmget error");
        return -1;
    }
    //2.建立映射关系,建立了共享内存和进程之间的映射关系。shmat 函数用于将共享内存映射到当前进程的地址空间中.如果 shmat 调用失败，它会返回 (void*)-1，并输出错误信息
    void* shm_start = shmat(shm_id, NULL, 0);
    /*
        shm_id：共享内存的ID，由上面的 shmget 返回。
        NULL：通常用于指定映射的地址，这里将其设为NULL，由系统自动选择地址。
        0：用于指定映射的选项
    */
    if (shm_start == (void*)-1) {
        perror("shmat error");
        return -1;
    }
	//3.进行内存操作
    while(1) {
    	//打印内存中的数据,shm_start 是共享内存的起始地址
        printf("[%s]\n", shm_start);
        sleep(1);
    }
    //4.解除映射关系,以避免内存泄漏。
    shmdt(shm_start);
    //5.删除共享内存
    shmctl(shm_id, IPC_RMID, NULL);
    
    return 0;
}
