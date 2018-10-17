#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

#include "pstat.h"

extern const int total_tickets;

int
sys_settickets(int ticCC)
{
	int ticC = ticCC;
  //ticC = 0
	if(argint(0, &ticC) < 0)
		return -1;

	acquire(&ptable.lock);
	setTic(proc, ticC);
	release(&ptable.lock);
	return 0;
}

int
sys_getpinfo(struct pstat* wantt)
{
	acquire(&ptable.lock);
	struct pstat* want = wantt;
  // dont have want
	if(argint(0, (int*)(&want)) < 0) {
		return -1;
  }
  struct proc* por;
	for(por = ptable.proc;por != &(ptable.proc[NPROC]); por++)
	{
		int porPlace = por - ptable.proc;
		if(por->state != UNUSED)
		{
			want->pid[porPlace] = por->pid;
			want->ticks[porPlace] = por->timeC;
			want->tickets[porPlace] = por->tic;
			want->inuse[porPlace] = por->inuse;
		}
	}
	release(&ptable.lock);
	return 0;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
