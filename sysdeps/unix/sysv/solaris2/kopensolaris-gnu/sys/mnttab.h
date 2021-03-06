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

#ifndef _MNTENT_H
#ifndef _SYS_MNTTAB_H
#define _SYS_MNTTAB_H

#include <features.h>
#include <paths.h>

#define MNTTAB	_PATH_MOUNTED
#define MNT_LINE_MAX	1024

#define MNT_TOOLONG	1
#define MNT_TOOMANY	2
#define MNT_TOOFEW	3

#define mntnull(mp)	((mp)->mnt_special = (mp)->mnt_mountp = \
	(mp)->mnt_fstype = (mp)->mnt_mntopts = (mp)->mnt_time = NULL)
#define putmntent(fd, mp)	(-1)

struct mnttab
  {
	char *mnt_special;
	char *mnt_mountp;
	char *mnt_fstype;
	char *mnt_mntopts;
	char *mnt_time;
  };

struct extmnttab
  {
	char *mnt_special;
	char *mnt_mountp;
	char *mnt_fstype;
	char *mnt_mntopts;
	char *mnt_time;
	unsigned int mnt_major;
	unsigned int mnt_minor;
  };

__BEGIN_DECLS

extern void	resetmnttab (FILE *);
# ifdef __REDIRECT_NTH
extern int __getmntent_sun (FILE *, struct mnttab *);
extern int __REDIRECT_NTH (getmntent, (FILE *, struct mnttab *),
	__getmntent_sun);
# else
#  define getmntent __getmntent_sun
# endif
extern int getextmntent (FILE *, struct extmnttab *, size_t);
extern int getmntany (FILE *, struct mnttab *, struct mnttab *);
# ifdef __REDIRECT_NTH
extern char	* __hasmntopt_sun (struct mnttab *, char *);
extern char * __REDIRECT_NTH (hasmntopt, (struct mnttab *, char *),
	__hasmntopt_sun);
# else
#  define hasmntopt __hasmntopt_sun
# endif
extern char	*mntopt (char **);

__END_DECLS

#endif /* _SYS_MNTTAB_H */
#endif
