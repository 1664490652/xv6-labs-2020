// user/pingpong.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    int pipe_p2c[2];
    int pipe_c2p[2];
    pipe(pipe_p2c);
    pipe(pipe_c2p);
    if(fork()==0)
    {
        close(pipe_p2c[1]); // 关闭不用的写端
        close(pipe_c2p[0]); // 关闭不用的读端

        // 子进程收到字符
        char buf = 0;
        read(pipe_p2c[0], &buf, 1);
        printf("<%d>: recieved ping\n", getpid());

        // 子进程发送字符
        write(pipe_c2p[1], &buf, 1);
        close(pipe_c2p[1]);
    }
    else
    {
        close(pipe_p2c[0]); // 关闭不用的读端
        close(pipe_c2p[1]); // 关闭不用的写端

        write(pipe_p2c[1], "a", 1);
        close(pipe_p2c[1]);

        // 父进程从子进程读取
        char buf = 0;
        read(pipe_c2p[0], &buf, 1);
        printf("<%d>: recieved pong\n", getpid());
        wait(0);
    }

    close(pipe_c2p[0]);
    close(pipe_p2c[0]);
    exit(0);
}