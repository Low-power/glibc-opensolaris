/* Copyright (C) 2002, 2003, 2005, 2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

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

#include <sysdep.h>
#include <shlib-compat.h>
#include <pthread-errnos.h>
#include <structsem.h>
#include <lowlevellock.h>


	.text

	.globl	__new_sem_post
	.type	__new_sem_post,@function
	.align	16
__new_sem_post:
	pushl	%ebx

	movl	8(%esp), %ebx

	LOCK
#if VALUE == 0
	addl	$1, (%ebx)
#else
	addl	$1, VALUE(%ebx)
#endif

	cmpl	$0, NWAITERS(%ebx)
	je	2f

	movl	$FUTEX_WAKE, %ecx
	orl	PRIVATE(%ebx), %ecx
	movl	$1, %edx
	movl	$SYS_futex, %eax
	ENTER_KERNEL

	testl	%eax, %eax
	js	1f

2:	xorl	%eax, %eax
	popl	%ebx
	ret

1:
#ifdef PIC
	call	__i686.get_pc_thunk.bx
#else
	movl	$4f, %ebx
4:
#endif
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
#if USE___THREAD
# ifdef NO_TLS_DIRECT_SEG_REFS
	movl	errno@gotntpoff(%ebx), %edx
	addl	%gs:0, %edx
	movl	$EINVAL, (%edx)
# else
	movl	errno@gotntpoff(%ebx), %edx
	movl	$EINVAL, %gs:(%edx)
# endif
#else
	call	__errno_location@plt
	movl	$EINVAL, (%eax)
#endif

	orl	$-1, %eax
	popl	%ebx
	ret
	.size	__new_sem_post,.-__new_sem_post
	versioned_symbol(libpthread, __new_sem_post, sem_post, GLIBC_2_1)
#if SHLIB_COMPAT(libpthread, GLIBC_2_0, GLIBC_2_1)
	.global	__old_sem_post
	.type	__old_sem_post,@function
__old_sem_post:
	pushl	%ebx

	movl	8(%esp), %ebx
	LOCK
	addl	$1, (%ebx)

	movl	$SYS_futex, %eax
	movl	$FUTEX_WAKE, %ecx
	movl	$1, %edx
	ENTER_KERNEL

	testl	%eax, %eax
	js	1b

	xorl	%eax, %eax
	popl	%ebx
	ret
	.size	__old_sem_post,.-__old_sem_post
	compat_symbol(libpthread, __old_sem_post, sem_post, GLIBC_2_0)
#endif


#ifdef PIC
	.section .gnu.linkonce.t.__i686.get_pc_thunk.bx,"ax",@progbits
	.globl	__i686.get_pc_thunk.bx
	.hidden	__i686.get_pc_thunk.bx
	.type	__i686.get_pc_thunk.bx,@function
__i686.get_pc_thunk.bx:
	movl (%esp), %ebx;
	ret
	.size	__i686.get_pc_thunk.bx,.-__i686.get_pc_thunk.bx
#endif
