#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void prime_proc(int fd)
{
    int p;
    if(read(fd, &p, sizeof(int)) == 0)  // 表示管道已关闭
    {
        close(fd);
        exit(0);
    }

    printf("prime %d\n", p);

    int p1[2];
    pipe(p1);

    if(fork()==0)
    {
        close(p1[1]);
        prime_proc(p1[0]);
    }
    else
    {
        int n;
        while(read(fd, &n, sizeof(int)))
        {
            if(n % p!=0)
            {
                write(p1[1], &n, sizeof(int));
            }
        }
        close(p1[1]);
        close(fd);
        wait(0);
        exit(0);
    }

}


int main(int args, int **argv)
{
    int p[2];
    pipe(p);
    if(fork()==0)  // 子进程
    {
        close(p[1]);
        prime_proc(p[0]);
    }
    else
    {
        close(p[0]);
        for(int i=2;i<=35;i++)
        {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);  // 等待子进程完成
        exit(0);
    }
    return 0;
}