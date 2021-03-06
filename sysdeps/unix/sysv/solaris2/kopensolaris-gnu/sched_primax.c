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

#include <sched.h>
#include <schedP.h>
#include <errno.h>
#include <sys/priocntl.h>

int
__sched_get_priority_max (int algorithm)
{
  pcpri_t pri;
  pri.pc_cid = __sched_policy_to_class (algorithm);
  if (pri.pc_cid == -1)
    {
      __set_errno (EINVAL);
      return -1;
    }

  int result = priocntl (0, 0, PC_GETPRIRANGE, &pri);
  if (result != 0)
    return -1;
  return pri.pc_clpmax;
}

weak_alias (__sched_get_priority_max, sched_get_priority_max)
