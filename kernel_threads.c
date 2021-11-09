
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
    while((PTCB*)tid->exit_cv != EXITED || (PTCB*)tid->detached != 1)
    {
      kernel_wait((PTCB*)tid->exit_cv, SCHED_USER);
    }
    
    if ((PTCB*)tid->detached == 1)
      return -1;
//	kernel_broadcast((PTCB*)tid->exit_cv);

  }
  return 0;
}

/**
  @brief Detach the given thread.
  */
int sys_ThreadDetach(Tid_t tid)
{
  (PTCB*)tid->detached = 1;
  kernel_broadcast((PTCB*)tid->exit_cv);
	return -1;
}

/**
  @brief Terminate the current thread.
  */
void sys_ThreadExit(int exitval)
{

  /*  First we need to kill thread and then check if this was the last thread to do following code */





  if (CURTHREAD->owner_pcb->thread_count == 0){   //if thread count is updated SOS

    /* Reparent any children of the exiting process to the 
       initial task */
    PCB* initpcb = get_pcb(1);
    while(!is_rlist_empty(& curproc->children_list)) {
      rlnode* child = rlist_pop_front(& curproc->children_list);
      child->pcb->parent = initpcb;
      rlist_push_front(& initpcb->children_list, child);
    }

    /* Add exited children to the initial task's exited list 
       and signal the initial task */
    if(!is_rlist_empty(& curproc->exited_list)) {
      rlist_append(& initpcb->exited_list, &curproc->exited_list);
      kernel_broadcast(& initpcb->child_exit);
    }

    /* Put me into my parent's exited list */
    rlist_push_front(& curproc->parent->exited_list, &curproc->exited_node);
    kernel_broadcast(& curproc->parent->child_exit);

  }

  assert(is_rlist_empty(& curproc->children_list));
  assert(is_rlist_empty(& curproc->exited_list));


  /* 
    Do all the other cleanup we want here, close files etc. 
   */

  /* Release the args data */
  if(curproc->args) {
    free(curproc->args);
    curproc->args = NULL;
  }

  /* Clean up FIDT */
  for(int i=0;i<MAX_FILEID;i++) {
    if(curproc->FIDT[i] != NULL) {
      FCB_decref(curproc->FIDT[i]);
      curproc->FIDT[i] = NULL;
    }
  }

  /* Disconnect my main_thread */
  curproc->main_thread = NULL;

  /* Now, mark the process as exited. */
  curproc->pstate = ZOMBIE;

  }
  else
  {


  }
  kernel_broadcast((PTCB*)tid->exit_cv);
}

