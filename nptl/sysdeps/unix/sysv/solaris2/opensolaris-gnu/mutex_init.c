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

#include <pthreadP.h>
#include <string.h>
#include <synch.h>
#include <synch_priv.h>

int mutex_init (mutex_t *mutex, int type, void * arg)
{
  // TODO: check type and ceil (arg)
  if (type & LOCK_ROBUST)
    {
      // TODO
    }
  else
    {
      memset (mutex, 0, sizeof(mutex_t));
    }
  mutex->mutex_type = type;
  mutex->mutex_flag = LOCK_INITED;
  mutex->mutex_magic = MUTEX_MAGIC;
  mutex->mutex_ceiling = *(int *)arg;

  return 0;
}