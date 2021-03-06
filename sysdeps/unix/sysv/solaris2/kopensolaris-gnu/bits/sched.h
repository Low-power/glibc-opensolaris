/* Definitions of constants and data structure for POSIX 1003.1b-1993
   scheduling interface. OpenSolaris version.
   Copyright (C) 1996-1999,2001-2003,2005,2006,2007,2008
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

#ifndef __need_schedparam

#ifndef _SCHED_H
# error "Never include <bits/sched.h> directly; use <sched.h> instead."
#endif


/* Scheduling algorithms.  */
#define SCHED_OTHER	0
#define SCHED_FIFO	1
#define SCHED_RR	2
#define SCHED_SYS	3
#define SCHED_IA	4
#define SCHED_FSS	5
#define SCHED_FX	6
#define _SCHED_NEXT	7

/* The official definition.  */
struct sched_param
  {
    int __sched_priority;
    int __sched_nicelim;
    int __sched_nice;
    int __sched_pad[6];
  };

#endif	/* need schedparam */

#if !defined __defined_schedparam \
    && (defined __need_schedparam || defined _SCHED_H)
# define __defined_schedparam	1
/* Data structure to describe a process' schedulability.  */
struct __sched_param
  {
    int __sched_priority;
    int __sched_nicelim;
    int __sched_nice;
    int __sched_pad[6];
  };
# undef __need_schedparam
#endif


#if defined _SCHED_H && !defined __cpu_set_t_defined
# define __cpu_set_t_defined
/* Size definition for CPU sets.  */
# define __CPU_SETSIZE	1024
# define __NCPUBITS	(8 * sizeof (__cpu_mask))

/* Type for array elements in 'cpu_set_t'.  */
typedef unsigned long int __cpu_mask;

/* Basic access functions.  */
# define __CPUELT(cpu)	((cpu) / __NCPUBITS)
# define __CPUMASK(cpu)	((__cpu_mask) 1 << ((cpu) % __NCPUBITS))

/* Data structure to describe CPU mask.  */
typedef struct
{
  __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

/* Access functions for CPU masks.  */
# if __GNUC_PREREQ (2, 91)
#  define __CPU_ZERO_S(setsize, cpusetp) \
  do __builtin_memset (cpusetp, '\0', setsize); while (0)
# else
#  define __CPU_ZERO_S(setsize, cpusetp) \
  do {									      \
    size_t __i;								      \
    size_t __imax = (setsize) / sizeof (__cpu_mask);			      \
    cpu_set_t *__arr = (cpusetp);					      \
    for (__i = 0; __i < __imax; ++__i)					      \
      __arr->__bits[__i] = 0;						      \
  } while (0)
# endif
# define __CPU_SET_S(cpu, setsize, cpusetp) \
  ({ size_t __cpu = (cpu);						      \
     __cpu < 8 * (setsize)						      \
     ? ((cpusetp)->__bits[__CPUELT (__cpu)] |= __CPUMASK (__cpu)) : 0; })
# define __CPU_CLR_S(cpu, setsize, cpusetp) \
  ({ size_t __cpu = (cpu);						      \
     __cpu < 8 * (setsize)						      \
     ? ((cpusetp)->__bits[__CPUELT (__cpu)] &= ~__CPUMASK (__cpu)) : 0; })
# define __CPU_ISSET_S(cpu, setsize, cpusetp) \
  ({ size_t __cpu = (cpu);						      \
     __cpu < 8 * (setsize)						      \
     ? (((cpusetp)->__bits[__CPUELT (__cpu)] & __CPUMASK (__cpu))) != 0 : 0; })

# define __CPU_COUNT_S(setsize, cpusetp) \
  __sched_cpucount (setsize, cpusetp)

# if __GNUC_PREREQ (2, 91)
#  define __CPU_EQUAL_S(setsize, cpusetp1, cpusetp2) \
  (__builtin_memcmp (cpusetp1, cpusetp2, setsize) == 0)
# else
#  define __CPU_EQUAL_S(setsize, cpusetp1, cpusetp2) \
  ({ cpu_set_t *__arr1 = (cpusetp1);					      \
     cpu_set_t *__arr2 = (cpusetp2);					      \
     size_t __imax = (setsize) / sizeof (__cpu_mask);			      \
     size_t __i;							      \
     for (__i = 0; __i < __imax; ++__i)					      \
       if (__arr1->__bits[__i] != __arr2->__bits[__i])			      \
	 break;								      \
     __i == __imax; })
# endif

# define __CPU_OP_S(setsize, destset, srcset1, srcset2, op) \
  ({ cpu_set_t *__dest = (destset);					      \
     cpu_set_t *__arr1 = (srcset1);					      \
     cpu_set_t *__arr2 = (srcset2);					      \
     size_t __imax = (setsize) / sizeof (__cpu_mask);			      \
     size_t __i;							      \
     for (__i = 0; __i < __imax; ++__i)					      \
       __dest->__bits[__i] = __arr1->__bits[__i] op __arr2->__bits[__i];      \
     __dest; })

# define __CPU_ALLOC_SIZE(count) \
  ((((count) + __NCPUBITS - 1) / __NCPUBITS) * 8)
# define __CPU_ALLOC(count) __sched_cpualloc (count)
# define __CPU_FREE(cpuset) __sched_cpufree (cpuset)

__BEGIN_DECLS

extern int __sched_cpucount (size_t __setsize, const cpu_set_t *__setp)
  __THROW;
extern cpu_set_t *__sched_cpualloc (size_t __count) __THROW __wur;
extern void __sched_cpufree (cpu_set_t *__set) __THROW;

__END_DECLS

#endif
