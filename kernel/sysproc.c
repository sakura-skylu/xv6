#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);//获取休眠时间
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;//获取当前时间
  while(ticks - ticks0 < n){//休眠小于n
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);//休眠
  }
  release(&tickslock);
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_sigalarm(void)
{
  int ticks;
  uint64 handler;
  struct  proc *p =myproc();

  argint(0,&ticks);
  argaddr(1,&handler); //分别取第0 1 个 

  p->alarm_interval = ticks; //闹钟周期
  p->alarm_handler = handler;
  p->alarm_info = 0;
  p->alarm_ticks = 0; //从头开始计数

  return 0;
}

uint64 sys_sigreturn(void)
{
  struct proc* p = myproc();
  memmove(p->trapframe,&p->alarm_tf,sizeof(struct trapframe));
  p->alarm_info = 0;
  return p->trapframe->a0;
}