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

#include <sysdep-cancel.h>
#include <inline-syscall.h>
#include <sys/socket.h>

extern int _so_accept(int sock, __SOCKADDR_ARG addr, unsigned int *addrlen,
    int version);

int
__libc_accept (fd, addr, addr_len)
     int fd;
     __SOCKADDR_ARG addr;
     socklen_t *addr_len;
{
  return _so_accept (fd, addr, addr_len, SOV_XPG4_2);
}

weak_alias (__libc_accept, accept)
libc_hidden_def (accept)
LIBC_CANCEL_HANDLED (); /* sys_accept handles cancellation */
