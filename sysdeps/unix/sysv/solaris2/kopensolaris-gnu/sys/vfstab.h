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

#ifndef _SYS_VFSTAB_H
#define _SYS_VFSTAB_H

#include <features.h>
#include <stdio.h>
#include <paths.h>

#define VFSTAB	_PATH_MNTTAB

#define VFS_LINE_MAX	1024

#define VFS_TOOLONG	1
#define VFS_TOOMANY	2
#define VFS_TOOFEW	3


struct vfstab
  {
	char *vfs_special;
	char *vfs_fsckdev;
	char *vfs_mountp;
	char *vfs_fstype;
	char *vfs_fsckpass;
	char *vfs_automnt;
	char *vfs_mntopts;
  };

__BEGIN_DECLS

extern int getvfsent (FILE *, struct vfstab *);
extern int getvfsspec (FILE *, struct vfstab *, char *);
extern int getvfsfile (FILE *, struct vfstab *, char *);
extern int getvfsany (FILE *, struct vfstab *, struct vfstab *);
extern void vfsnull (FILE *, struct vfstab *);
extern void putvfsent (FILE *, struct vfstab *);

__END_DECLS

#endif /* _SYS_VFSTAB_H */
