#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char **argv)
{
    char buf[512];
    char* exe_argv[MAXARG];
    int i;

    // 把命令参数复制到exe_argv中，如echo bye
    for(i=0;i<argc-1;i++){
        exe_argv[i]=argv[i+1];
    }

    int arg_start = i;
    int n=0;
    while(read(0, &buf[n], 1)==1){
        if(buf[n]=='\n'){
            buf[n]=0;
            exe_argv[arg_start]=buf;
            exe_argv[arg_start+1]=0;  // ={"echo", "bye", "hello", 0};

            if(fork()==0){
                exec(exe_argv[0], exe_argv);
                exit(1);  //exec失败
            }
            else
            {
                wait(0);
            }
            n=0;  // 清空buffer,下次从buf[0]开始读
        }
        else
        {
            n++;
        }
    }
    exit(0);
}
