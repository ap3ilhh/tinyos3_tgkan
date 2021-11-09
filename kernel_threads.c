
#include "tinyos.h"
#include "kernel_sched.h"
#include "kernel_proc.h"

/** 
  @brief Create a new thread in the current process.
  */
Tid_t sys_CreateThread(Task task, int argl, void* args)
{
  PCB *curproc;
  PTCB *newptcb;
  TCB *newtcb;

  curproc = CURPROC;

  if(call != NULL) {

    newtcb = spawn_thread(curproc, start_new_thread); 
    newptcb = acquire_PTCB();

    newptcb->task = task;
    newptcb->argl = argl;
    newptcb->args = args;

    newptcb->tcb = newtcb;
    newptcb->exited = 0;
    newptcb->detached = 0;
    newptcb->refcount = 0;
    newptcb->exit_cv = COND_INIT;
      ////////////xreiazetai kai exitval arxikopoihsh?????
    newtcb->ptcb = newptcb;
    curproc->thread_count++;

    rlist_push_front(& CURTHREAD->ptcb->ptcb_list_node, & newptcb->ptcb_list_node);
    wakeup(newtcb);
    
    return  (Tid_t) newptcb;
  }
  else
  	return NOTHREAD;
}

/**
  @brief Return the Tid of the current thread.
 */
Tid_t sys_ThreadSelf()

	return (Tid_t) cur_thread()->ptcb;
}

/**
  @brief Join the given thread.
  */
int sys_ThreadJoin(Tid_t tid, int* exitval)
{
  if((PTCB*)tid->detached == 0)
  {
    (PTCB*)tid->refcount++;
    kernel_wait((PTCB*)tid->exit_cv, SCHED_USER);
	

  }
  return -1;
}

/**
  @brief Detach the given thread.
  */
int sys_ThreadDetach(Tid_t tid)
{
	return -1;
}

/**
  @brief Terminate the current thread.
  */
void sys_ThreadExit(int exitval)
{

}

