/* Copyright (C) 2002, 2003, 2007, 2008 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.
   OpenSolaris bits contributed by David Bartley
    <dtbartle@csclub.uwaterloo.ca>, 2008.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sysdep.h>
#include <libio/libioP.h>
#include <tls.h>
#include "fork.h"
#include <hp-timing.h>
#include <ldsodefs.h>
#include <bits/stdio-lock.h>
#include <atomic.h>
#include <inline-syscall.h>

DECLARE_INLINE_SYSCALL (int, lwp_self, void);
DECLARE_INLINE_SYSCALL (int64_t, forkx, int flags);


unsigned long int *__fork_generation_pointer;



/* The single linked list of all currently registered for handlers.  */
struct fork_handler *__fork_handlers;


static void
fresetlockfiles (void)
{
  _IO_ITER i;

  for (i = _IO_iter_begin(); i != _IO_iter_end(); i = _IO_iter_next(i))
    _IO_lock_init (*((_IO_lock_t *) _IO_iter_file(i)->_lock));
}


pid_t
__libc_fork (void)
{
  pid_t pid;
  struct used_handler
  {
    struct fork_handler *handler;
    struct used_handler *next;
  } *allp = NULL;

  /* Run all the registered preparation handlers.  In reverse order.
     While doing this we build up a list of all the entries.  */
  struct fork_handler *runp;
  while ((runp = __fork_handlers) != NULL)
    {
      unsigned int oldval = runp->refcntr;

      if (oldval == 0)
	/* This means some other thread removed the list just after
	   the pointer has been loaded.  Try again.  Either the list
	   is empty or we can retry it.  */
	continue;

      /* Bump the reference counter.  */
      if (atomic_compare_and_exchange_bool_acq (&__fork_handlers->refcntr,
						oldval + 1, oldval))
	/* The value changed, try again.  */
	continue;

      /* We bumped the reference counter for the first entry in the
	 list.  That means that none of the following entries will
	 just go away.  The unloading code works in the order of the
	 list.

         While executing the registered handlers we are building a
         list of all the entries so that we can go backward later on.  */
      while (1)
	{
	  /* Execute the handler if there is one.  */
	  if (runp->prepare_handler != NULL)
	    runp->prepare_handler ();

	  /* Create a new element for the list.  */
	  struct used_handler *newp
	    = (struct used_handler *) alloca (sizeof (*newp));
	  newp->handler = runp;
	  newp->next = allp;
	  allp = newp;

	  /* Advance to the next handler.  */
	  runp = runp->next;
	  if (runp == NULL)
	    break;

	  /* Bump the reference counter for the next entry.  */
	  atomic_increment (&runp->refcntr);
	}

      /* We are done.  */
      break;
    }

  _IO_list_lock ();

  rval_t result;
  result.r_vals = INLINE_SYSCALL (forkx, 1, 0);
  if(result.r_vals == -1)
    pid = (pid_t)-1;
  else if(result.r_val2)
    pid = 0;
  else
    pid = (pid_t)result.r_val1;

  if (pid == 0)
    {
      struct pthread *self = THREAD_SELF;

      if (__fork_generation_pointer != NULL)
	*__fork_generation_pointer += 4;

      /* Adjust the PID field for the new process.  */
      THREAD_SETMEM (self, pid, getpid());
      int tid = INLINE_SYSCALL (lwp_self, 0);
      THREAD_SETMEM (self, tid, tid);

#if HP_TIMING_AVAIL
      /* The CPU clock of the thread and process have to be set to zero.  */
      hp_timing_t now;
      HP_TIMING_NOW (now);
      THREAD_SETMEM (self, cpuclock_offset, now);
      GL(dl_cpuclock_offset) = now;
#endif

      /* Reset the file list.  These are recursive mutexes.  */
      fresetlockfiles ();

      /* Reset locks in the I/O code.  */
      _IO_list_resetlock ();

      /* Reset the lock the dynamic loader uses to protect its data.  */
      __rtld_lock_initialize (GL(dl_load_lock));

      /* Run the handlers registered for the child.  */
      while (allp != NULL)
	{
	  if (allp->handler->child_handler != NULL)
	    allp->handler->child_handler ();

	  /* Note that we do not have to wake any possible waiter.
 	     This is the only thread in the new process.  The count
 	     may have been bumped up by other threads doing a fork.
 	     We reset it to 1, to avoid waiting for non-existing
 	     thread(s) to release the count.  */
	  allp->handler->refcntr = 1;

	  /* XXX We could at this point look through the object pool
	     and mark all objects not on the __fork_handlers list as
	     unused.  This is necessary in case the fork() happened
	     while another thread called dlclose() and that call had
	     to create a new list.  */

	  allp = allp->next;
	}

      /* Initialize the fork lock.  */
      __libc_lock_init(__fork_lock);
    }
  else
    {
      /* We execute this even if the 'fork' call failed.  */
      _IO_list_unlock ();

      /* Run the handlers registered for the parent.  */
      while (allp != NULL)
	{
	  if (allp->handler->parent_handler != NULL)
	    allp->handler->parent_handler ();

#if 0 // TODO
	  if (atomic_decrement_and_test (&allp->handler->refcntr)
	      && allp->handler->need_signal)
	    lll_futex_wake (allp->handler->refcntr, 1, LLL_PRIVATE);
#endif

	  allp = allp->next;
	}
    }

  return pid;
}
weak_alias (__libc_fork, __fork)
libc_hidden_def (__fork)
weak_alias (__libc_fork, fork)