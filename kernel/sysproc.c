#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "fcntl.h"
#include "file.h"
#include "sysfile.c"
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
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


uint64
sys_mmap(void)
{
  uint64 addr;
  int len;
  int prot;
  int flags;
  int fd;
  int offset;
  struct file *f;

  argaddr(0, &addr);
  argint(1, &len);
  argint(2, &prot);
  argint(3, &flags);
  argfd(4, &fd, &f);
  argint(5, &offset);

  if(addr != 0)
    return -1;

  if(len <= 0)
    return -1;

  struct proc* p = myproc();
  struct vma* v = 0;
  for(int i=0;i<NVMA;i++){
    if(p->vmas[i].used == 0){
      v = &p->vmas[i];
      break;
    }
  }
  if(v==0){
    return -1;
  }

  if((prot&PROT_WRITE) && flags == MAP_SHARED && f->writable == 0){
    return -1;
  }

  uint64 mapaddr = TRAPFRAME - PGSIZE;

  for(int i = 0; i < NVMA; i++){
    if(p->vmas[i].used){
      if(p->vmas[i].addr <= mapaddr)
        mapaddr = p->vmas[i].addr - PGROUNDUP(len);
    }
  }

  mapaddr = PGROUNDDOWN(mapaddr - PGROUNDUP(len));

  v->used = 1;
  v->addr = mapaddr;
  v->len = PGROUNDUP(len);
  v->prot = prot;
  v->flags = flags;
  v->file = f;
  v->offset = offset;

  filedup(f);

  return mapaddr;
}

uint64
sys_munmap(void)
{
  return -1;
}