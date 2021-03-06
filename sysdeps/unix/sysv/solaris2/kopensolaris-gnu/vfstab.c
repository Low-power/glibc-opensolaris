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

#include <stdio.h>
#include <sys/vfstab.h>
#include <string.h>

#define _VFS_DELIM		" \t"
#define _VFS_CMP(x, y, f)	(!y->f || (y->f && strcmp (x->f, y->f) == 0))

static __thread char vfs_buf[VFS_LINE_MAX + 2];


static int vfs_strtok_r (char *str, const char *delim,
      char **saveptr, char **strt)
{
  int ret = (*strt = strtok_r (str, delim, saveptr)) ? 1 : 0;
  if (*strt && strcmp (*strt, "-") == 0)
    *strt = NULL;
  return ret;
}


int getvfsent (FILE *fp, struct vfstab *vp)
{
  while (fgets (vfs_buf, VFS_LINE_MAX + 2, fp) != NULL)
    {
      /* Check for long lines.  */
      size_t len = strlen (vfs_buf);
      if (len > VFS_LINE_MAX)
        return VFS_TOOLONG;

      /* Trim leading spaces/tabs.  */
      char *bufp = vfs_buf;
      while (*bufp == ' ' || *bufp == '\t')
        bufp++;
      len = strlen (bufp);

      /* Ignore empty lines and lines starting with #.  */
      if (len && (bufp[0] == '\n' || bufp[0] == '#'))
        continue;

      bufp[len - 1] = '\0';

      /* Tokenize.  */
      char *tokp = NULL;
      if (!vfs_strtok_r (bufp, _VFS_DELIM, &tokp, &vp->vfs_special) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_fsckdev) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_mountp) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_fstype) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_fsckpass) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_automnt) ||
          !vfs_strtok_r (NULL, _VFS_DELIM, &tokp, &vp->vfs_mntopts))
        return VFS_TOOFEW;

      /* If we can still tokenize the line is too long.  */
      if (strtok_r (NULL, _VFS_DELIM, &tokp))
        return VFS_TOOMANY;

      return 0;
    }

  return -1;
}


int getvfsany (FILE *fp, struct vfstab *vp, struct vfstab *vpref)
{
  int res;
  while ((res = getvfsent (fp, vp)) == 0)
    {
      if (_VFS_CMP (vp, vpref, vfs_special) &&
          _VFS_CMP (vp, vpref, vfs_fsckdev) &&
          _VFS_CMP (vp, vpref, vfs_mountp) &&
          _VFS_CMP (vp, vpref, vfs_fstype) &&
          _VFS_CMP (vp, vpref, vfs_fsckpass) &&
          _VFS_CMP (vp, vpref, vfs_automnt) &&
          _VFS_CMP (vp, vpref, vfs_mntopts))
        return 0;
    }

  return res;
}


int getvfsfile (FILE *fp, struct vfstab *vp, char *file)
{
  struct vfstab vref = {
   .vfs_mountp = file
  };
  return getvfsany (fp, vp, &vref);
}


int getvfsspec (FILE *fp, struct vfstab *vp, char *spec)
{
  struct vfstab vref = {
    .vfs_special = spec
  };
  return getvfsany (fp, vp, &vref);
}
