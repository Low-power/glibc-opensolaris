/* Copyright (C) 1993, 1995-2003, 2004, 2006, 2007
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#include <alloca.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>

#include <sysdep.h>
#include <sys/syscall.h>
#include <bp-checks.h>

#ifndef __KERNEL_DIRENT
#define __KERNEL_DIRENT
struct kernel_dirent
  {
    __ino_t d_ino;
    __off_t d_off;
    unsigned short d_reclen;
    char d_name[1];
  };
#endif

#ifndef __SYSCALL_GETDENTS
DECLARE_INLINE_SYSCALL(int, getdents, int, struct kernel_dirent *, size_t);
#define __SYSCALL_GETDENTS __syscall_getdents
#endif
#ifndef __GETDENTS
# define __GETDENTS __getdents
#endif
#ifndef DIRENT_TYPE
# define DIRENT_TYPE struct dirent
#endif
#ifndef DIRENT_SET_DP_INO
# define DIRENT_SET_DP_INO(dp, value) (dp)->d_ino = (value)
#endif

/* The problem here is that we cannot simply read the next NBYTES
   bytes.  We need to take the additional field into account.  We use
   some heuristic.  Assuming the directory contains names with 14
   characters on average we can compute an estimated number of entries
   which fit in the buffer.  Taking this number allows us to specify a
   reasonable number of bytes to read.  If we should be wrong, we can
   reset the file descriptor.  In practice the kernel is limiting the
   amount of data returned much more then the reduced buffer size.  */
ssize_t
internal_function
__GETDENTS (int fd, char *buf, size_t nbytes)
{
  ssize_t retval;

  retval = __SYSCALL_GETDENTS (fd, (struct kernel_dirent *)CHECK_N(buf, nbytes), nbytes);

  /* The kernel added the d_type value after the name.  Change
	 this now.  */
  if (retval != -1)
	{
	  union
	  {
	    struct kernel_dirent k;
	    struct dirent u;
	  } *kbuf = (void *) buf;

	  while ((char *) kbuf < buf + retval)
	    {
	      char d_type = *((char *) kbuf + kbuf->k.d_reclen - 1);
	      memmove (kbuf->u.d_name, kbuf->k.d_name,
		       strlen (kbuf->k.d_name) + 1);
	      kbuf->u.d_type = d_type;

	      kbuf = (void *) ((char *) kbuf + kbuf->k.d_reclen);
	    }
	}

  return retval;
}
