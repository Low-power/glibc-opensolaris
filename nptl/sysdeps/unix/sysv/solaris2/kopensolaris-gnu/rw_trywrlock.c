/* Copyright (C) 2008 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by David Bartley <dtbartle@csclub.uwaterloo.ca>, 2008.

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
#include <pthreadP.h>
#include <synch.h>


int rw_trywrlock (rwlock)
      rwlock_t *rwlock;
{
  int errval = mutex_trylock (&rwlock->mutex);
  if (errval != 0)
    return errval;

  if (rwlock->readers)
    {
      (void)mutex_unlock (&rwlock->mutex);
      return EBUSY;
    }

  /* Set write lock.  */
  rwlock->readers = _RWLOCK_WR_LOCK;
  rwlock->owner = (uintptr_t)THREAD_SELF;
  if (rwlock->type & LOCK_SHARED)
    rwlock->ownerpid = THREAD_GETMEM (THREAD_SELF, pid);

  (void)mutex_unlock (&rwlock->mutex);
  return 0;
}
