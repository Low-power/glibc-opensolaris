/* Declarations of rctl functions and types.
   Copyright (C) 2008 Free Software Foundation, Inc.
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

#ifndef _SYS_RCTL_H
#define _SYS_RCTL_H

#include <features.h>

#define RCTL_LOCAL_NOACTION	0x00000000
#define RCTL_LOCAL_SIGNAL	0x00000001
#define RCTL_LOCAL_DENY		0x00000002
#define RCTL_LOCAL_MAXIMAL	0x80000000
#define RCTL_LOCAL_PROJDB	0x40000000
#define RCTL_LOCAL_ACTION_MASK	0xFFFF0000
#define RCTL_LOCAL_MASK		0xC0000003

#define RCTL_GLOBAL_NOACTION		0x00000000
#define RCTL_GLOBAL_SYSLOG		0x00000001
#define RCTL_GLOBAL_NOBASIC		0x80000000
#define RCTL_GLOBAL_LOWERABLE		0x40000000
#define RCTL_GLOBAL_DENY_ALWAYS		0x20000000
#define RCTL_GLOBAL_DENY_NEVER		0x10000000
#define RCTL_GLOBAL_FILE_SIZE		0x08000000
#define RCTL_GLOBAL_CPU_TIME		0x04000000
#define RCTL_GLOBAL_SIGNAL_NEVER	0x02000000
#define RCTL_GLOBAL_NOLOCALACTION	RCTL_GLOBAL_SIGNAL_NEVER
#define RCTL_GLOBAL_INFINITE		0x01000000
#define RCTL_GLOBAL_UNOBSERVABLE	0x00800000
#define RCTL_GLOBAL_SYSLOG_NEVER	0x00080000
#define RCTL_GLOBAL_BYTES		0x00400000
#define RCTL_GLOBAL_SECONDS		0x00200000
#define RCTL_GLOBAL_COUNT		0x00100000
#define RCTL_GLOBAL_ACTION_MASK		0xFFFF0000
#define RCTL_GLOBAL_MASK		0xFFF80001

#define RCTL_FIRST	0x00000000
#define RCTL_NEXT	0x00000001
#define RCTL_USAGE	0x00000002

#define RCTL_INSERT	0x00000000
#define RCTL_DELETE	0x00000001
#define RCTL_REPLACE	0x00000002

#define RCTL_USE_RECIPIENT_PID  0x10000000

#define RCTLSYS_ACTION_MASK	0xFFFF0000
#define RCTLSYS_MASK		0x10000003

#define RCPRIV_BASIC		0x01000000
#define RCPRIV_PRIVILEGED	0x04000000
#define RCPRIV_SYSTEM		0x07000000

typedef enum
  {
    RCENTITY_PROCESS,
    RCENTITY_TASK,
    RCENTITY_PROJECT,
    RCENTITY_ZONE
  } rctl_entity_t;
#define RC_MAX_ENTITY	RCENTITY_ZONE

typedef unsigned long long rctl_qty_t;
typedef int rctl_priv_t;
typedef struct rctlblk rctlblk_t;
typedef struct rctl_set rctl_set_t;

__BEGIN_DECLS

int setrctl (const char *controlname, rctlblk_t *old_blk, rctlblk_t *new_blk,
    unsigned int flags);

int getrctl (const char *controlname, rctlblk_t *old_blk, rctlblk_t *new_blk,
    unsigned int flags);

__END_DECLS

#endif /* _SYS_RCTL_H */
