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

#include <inline-syscall.h>
#include <stropts.h>

DECLARE_INLINE_SYSCALL (int, putpmsg, int fildes, const struct strbuf *ctlptr,
    const struct strbuf *dataptr, int band, int flags);

int
putpmsg (fildes, ctlptr, dataptr, band, flags)
     int fildes;
     const struct strbuf *ctlptr;
     const struct strbuf *dataptr;
     int band;
     int flags;
{
  return INLINE_SYSCALL (putpmsg, 4, fildes, ctlptr, dataptr, band,
      flags | MSG_XPG4);
}

