/* Copyright (C) 2002, 2003, 2004, 2008 Free Software Foundation, Inc.
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

#include <errno.h>
#include <signal.h>
#include <pthreadP.h>
#include <sysdep.h>


int
pthread_sigmask (how, newmask, oldmask)
     int how;
     const sigset_t *newmask;
     sigset_t *oldmask;
{
  sigset_t local_newmask;

  if (how != SIG_BLOCK && how != SIG_SETMASK && how != SIG_UNBLOCK)
    return EINVAL;

  /* The only thing we have to make sure here is that SIGCANCEL is
     not blocked.  */
  if (newmask != NULL
	  && __builtin_expect (__sigismember (newmask, SIGCANCEL), 0))
    {
      local_newmask = *newmask;
      __sigdelset (&local_newmask, SIGCANCEL);
      newmask = &local_newmask;
    }

  return sigprocmask (how, newmask, oldmask) == -1 ? errno : 0;
}
