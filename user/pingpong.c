#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char* argv[]){
    int p1[2];//父->子
    int p2[2];//子->父
    char buf='x';

    pipe(p1);
    pipe(p2);

    int pid = fork();
    if(pid==0){
        close(p1[1]);
        close(p2[0]);
        read(p1[0],&buf,1);//父的输出
        printf("%d: received ping\n",getpid());
        write(p2[1],&buf,1);

        close(p1[0]);
        close(p2[1]);
    }else{
        close(p1[0]);
        close(p2[1]);

        write(p1[1],&buf,1);
        read(p2[0],&buf,1);
        printf("%d: received pong\n",getpid());
        close(p1[1]);
        close(p2[0]);
        
        wait(0);
        exit(0);
    }
}