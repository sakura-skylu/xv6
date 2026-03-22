#include<kernel/types.h>
#include<user/user.h>
#include "kernel/stat.h"

void sieve(int leftfd){
    int prime;
    int num;

    if(read(leftfd,&prime,sizeof(int))==0){
        close(leftfd);
        return;
    }

    printf("prime %d\n",prime);

    int p[2];
    pipe(p);

    int pid = fork();
    if(pid==0){
        close(leftfd);
        close(p[1]);
        sieve(p[0]);
    }else{
        close(p[0]);
        while((read(leftfd,&num,sizeof(int)))>0){
            //必须处理管道缓冲区的“所有输入数据”
            if(num%prime!=0){
                write(p[1],&num,sizeof(int)); 
            }
        }//把那些“不能被 prime 整除”的数，发给下一层
        close(leftfd);
        close(p[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc,char* argv[]){
    int p[2];
    pipe(p);
    int pid = fork();

    if(pid==0){
        close(p[1]);//只读
        sieve(p[0]);
    }else{
        close(p[0]);
        for(int i=2;i<=35;i++){
            write(p[1],&i,sizeof(int));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    return 0;
}