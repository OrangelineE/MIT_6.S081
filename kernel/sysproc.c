#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
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
sys_trace(void)
{
  int t_mask;
  //get the mask through argint()
  //0: get the first argument from the user space
  if(argint(0, &t_mask) < 0)
    return -1; 
  //give the mask value to the current process
  myproc()->mask = t_mask;
  // printf("t_mask is %d\n", t_mask);
  //don't forget the return value!
  return 0;
}

extern int collect_free_mem(void);
extern int collect_using_proc(void);

uint64
sys_info(void)
{
  struct proc *p = myproc();
  uint64 si; // user pointer to struct sysinfo

  struct sysinfo info;
  info.freemem = collect_free_mem();
  info.nproc =  collect_using_proc();

  if(argaddr(0, &si) < 0) 
    return -1;
  //copy the struct sysinfo back to user space
  if(copyout(p->pagetable, si, (char *)&info, sizeof(info)) < 0)
      return -1;
    return 0;
}
