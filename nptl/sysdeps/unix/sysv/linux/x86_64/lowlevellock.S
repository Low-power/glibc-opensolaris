/* Copyright (C) 2002-2006, 2007 Free Software Foundation, Inc.
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
#include <pthread-errnos.h>
#include <kernel-features.h>
#include <lowlevellock.h>

	.text

#ifdef __ASSUME_PRIVATE_FUTEX
# define LOAD_PRIVATE_FUTEX_WAIT(reg) \
	movl	$(FUTEX_WAIT | FUTEX_PRIVATE_FLAG), reg
# define LOAD_PRIVATE_FUTEX_WAKE(reg) \
	movl	$(FUTEX_WAKE | FUTEX_PRIVATE_FLAG), reg
# define LOAD_FUTEX_WAIT(reg) \
	xorl	$(FUTEX_WAIT | FUTEX_PRIVATE_FLAG), reg
# define LOAD_FUTEX_WAKE(reg) \
	xorl	$(FUTEX_WAKE | FUTEX_PRIVATE_FLAG), reg
#else
# if FUTEX_WAIT == 0
#  define LOAD_PRIVATE_FUTEX_WAIT(reg) \
	movl    %fs:PRIVATE_FUTEX, reg
# else
#  define LOAD_PRIVATE_FUTEX_WAIT(reg) \
	movl	%fs:PRIVATE_FUTEX, reg ; \
	orl	$FUTEX_WAIT, reg
# endif
# define LOAD_PRIVATE_FUTEX_WAKE(reg) \
	movl    %fs:PRIVATE_FUTEX, reg ; \
	orl     $FUTEX_WAKE, reg
# if FUTEX_WAIT == 0
#  define LOAD_FUTEX_WAIT(reg) \
	xorl	$FUTEX_PRIVATE_FLAG, reg ; \
	andl	%fs:PRIVATE_FUTEX, reg
# else
#  define LOAD_FUTEX_WAIT(reg) \
	xorl	$FUTEX_PRIVATE_FLAG, reg ; \
	andl	%fs:PRIVATE_FUTEX, reg ; \
	orl	$FUTEX_WAIT, reg
# endif
# define LOAD_FUTEX_WAKE(reg) \
	xorl	$FUTEX_PRIVATE_FLAG, reg ; \
	andl	%fs:PRIVATE_FUTEX, reg ; \
	orl	$FUTEX_WAKE, reg
#endif


/* For the calculation see asm/vsyscall.h.  */
#define VSYSCALL_ADDR_vgettimeofday	0xffffffffff600000


	.globl	__lll_lock_wait_private
	.type	__lll_lock_wait_private,@function
	.hidden	__lll_lock_wait_private
	.align	16
__lll_lock_wait_private:
	cfi_startproc
	pushq	%r10
	cfi_adjust_cfa_offset(8)
	pushq	%rdx
	cfi_adjust_cfa_offset(8)
	cfi_offset(%r10, -16)
	cfi_offset(%rdx, -24)
	xorq	%r10, %r10	/* No timeout.  */
	movl	$2, %edx
	LOAD_PRIVATE_FUTEX_WAIT (%esi)

	cmpl	%edx, %eax	/* NB:	 %edx == 2 */
	jne	2f

1:	movl	$SYS_futex, %eax
	syscall

2:	movl	%edx, %eax
	xchgl	%eax, (%rdi)	/* NB:	 lock is implied */

	testl	%eax, %eax
	jnz	1b

	popq	%rdx
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rdx)
	popq	%r10
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r10)
	retq
	cfi_endproc
	.size	__lll_lock_wait_private,.-__lll_lock_wait_private

#ifdef NOT_IN_libc
	.globl	__lll_lock_wait
	.type	__lll_lock_wait,@function
	.hidden	__lll_lock_wait
	.align	16
__lll_lock_wait:
	cfi_startproc
	pushq	%r10
	cfi_adjust_cfa_offset(8)
	pushq	%rdx
	cfi_adjust_cfa_offset(8)
	cfi_offset(%r10, -16)
	cfi_offset(%rdx, -24)
	xorq	%r10, %r10	/* No timeout.  */
	movl	$2, %edx
	LOAD_FUTEX_WAIT (%esi)

	cmpl	%edx, %eax	/* NB:	 %edx == 2 */
	jne	2f

1:	movl	$SYS_futex, %eax
	syscall

2:	movl	%edx, %eax
	xchgl	%eax, (%rdi)	/* NB:	 lock is implied */

	testl	%eax, %eax
	jnz	1b

	popq	%rdx
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rdx)
	popq	%r10
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r10)
	retq
	cfi_endproc
	.size	__lll_lock_wait,.-__lll_lock_wait

	.globl	__lll_timedlock_wait
	.type	__lll_timedlock_wait,@function
	.hidden	__lll_timedlock_wait
	.align	16
__lll_timedlock_wait:
	cfi_startproc
	/* Check for a valid timeout value.  */
	cmpq	$1000000000, 8(%rdx)
	jae	3f

	pushq	%r8
	cfi_adjust_cfa_offset(8)
	pushq	%r9
	cfi_adjust_cfa_offset(8)
	pushq	%r12
	cfi_adjust_cfa_offset(8)
	pushq	%r13
	cfi_adjust_cfa_offset(8)
	pushq	%r14
	cfi_adjust_cfa_offset(8)
	cfi_offset(%r8, -16)
	cfi_offset(%r9, -24)
	cfi_offset(%r12, -32)
	cfi_offset(%r13, -40)
	cfi_offset(%r14, -48)
	pushq	%rsi
	cfi_adjust_cfa_offset(8)

	/* Stack frame for the timespec and timeval structs.  */
	subq	$24, %rsp
	cfi_adjust_cfa_offset(24)

	movq	%rdi, %r12
	movq	%rdx, %r13

1:
	/* Get current time.  */
	movq	%rsp, %rdi
	xorl	%esi, %esi
	movq	$VSYSCALL_ADDR_vgettimeofday, %rax
	/* This is a regular function call, all caller-save registers
	   might be clobbered.  */
	callq	*%rax

	/* Compute relative timeout.  */
	movq	8(%rsp), %rax
	movl	$1000, %edi
	mul	%rdi		/* Milli seconds to nano seconds.  */
	movq	(%r13), %rdi
	movq	8(%r13), %rsi
	subq	(%rsp), %rdi
	subq	%rax, %rsi
	jns	4f
	addq	$1000000000, %rsi
	decq	%rdi
4:	testq	%rdi, %rdi
	js	5f		/* Time is already up.  */

	/* Futex call.  */
	movq	%rdi, (%rsp)	/* Store relative timeout.  */
	movq	%rsi, 8(%rsp)

	movl	$1, %eax
	movl	$2, %edx
	LOCK
	cmpxchgl %edx, (%r12)

	testl	%eax, %eax
	je	8f

	movq	%rsp, %r10
	movl	24(%rsp), %esi
	LOAD_FUTEX_WAIT (%esi)
	movq	%r12, %rdi
	movl	$SYS_futex, %eax
	syscall
	movq	%rax, %rcx

8:				/* NB: %edx == 2 */
	xorl	%eax, %eax
	LOCK
	cmpxchgl %edx, (%r12)
	jnz	7f

6:	addq	$32, %rsp
	cfi_adjust_cfa_offset(-32)
	popq	%r14
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r14)
	popq	%r13
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r13)
	popq	%r12
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r12)
	popq	%r9
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r9)
	popq	%r8
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r8)
	retq

3:	movl	$EINVAL, %eax
	retq

	cfi_adjust_cfa_offset(72)
	cfi_offset(%r8, -16)
	cfi_offset(%r9, -24)
	cfi_offset(%r12, -32)
	cfi_offset(%r13, -40)
	cfi_offset(%r14, -48)
	/* Check whether the time expired.  */
7:	cmpq	$-ETIMEDOUT, %rcx
	je	5f

	/* Make sure the current holder knows we are going to sleep.  */
	movl	%edx, %eax
	xchgl	%eax, (%rdi)
	testl	%eax, %eax
	jz	6b
	jmp	1b

5:	movl	$ETIMEDOUT, %eax
	jmp	6b
	cfi_endproc
	.size	__lll_timedlock_wait,.-__lll_timedlock_wait
#endif


	.globl	__lll_unlock_wake_private
	.type	__lll_unlock_wake_private,@function
	.hidden	__lll_unlock_wake_private
	.align	16
__lll_unlock_wake_private:
	cfi_startproc
	pushq	%rsi
	cfi_adjust_cfa_offset(8)
	pushq	%rdx
	cfi_adjust_cfa_offset(8)
	cfi_offset(%rsi, -16)
	cfi_offset(%rdx, -24)

	movl	$0, (%rdi)
	LOAD_PRIVATE_FUTEX_WAKE (%esi)
	movl	$1, %edx	/* Wake one thread.  */
	movl	$SYS_futex, %eax
	syscall

	popq	%rdx
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rdx)
	popq	%rsi
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rsi)
	retq
	cfi_endproc
	.size	__lll_unlock_wake_private,.-__lll_unlock_wake_private

#ifdef NOT_IN_libc
	.globl	__lll_unlock_wake
	.type	__lll_unlock_wake,@function
	.hidden	__lll_unlock_wake
	.align	16
__lll_unlock_wake:
	cfi_startproc
	pushq	%rsi
	cfi_adjust_cfa_offset(8)
	pushq	%rdx
	cfi_adjust_cfa_offset(8)
	cfi_offset(%rsi, -16)
	cfi_offset(%rdx, -24)

	movl	$0, (%rdi)
	LOAD_FUTEX_WAKE (%esi)
	movl	$1, %edx	/* Wake one thread.  */
	movl	$SYS_futex, %eax
	syscall

	popq	%rdx
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rdx)
	popq	%rsi
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%rsi)
	retq
	cfi_endproc
	.size	__lll_unlock_wake,.-__lll_unlock_wake

	.globl	__lll_timedwait_tid
	.type	__lll_timedwait_tid,@function
	.hidden	__lll_timedwait_tid
	.align	16
__lll_timedwait_tid:
	cfi_startproc
	pushq	%r12
	cfi_adjust_cfa_offset(8)
	pushq	%r13
	cfi_adjust_cfa_offset(8)
	cfi_offset(%r12, -16)
	cfi_offset(%r13, -24)

	movq	%rdi, %r12
	movq	%rsi, %r13

	subq	$16, %rsp
	cfi_adjust_cfa_offset(16)

	/* Get current time.  */
2:	movq	%rsp, %rdi
	xorl	%esi, %esi
	movq	$VSYSCALL_ADDR_vgettimeofday, %rax
	callq	*%rax

	/* Compute relative timeout.  */
	movq	8(%rsp), %rax
	movl	$1000, %edi
	mul	%rdi		/* Milli seconds to nano seconds.  */
	movq	(%r13), %rdi
	movq	8(%r13), %rsi
	subq	(%rsp), %rdi
	subq	%rax, %rsi
	jns	5f
	addq	$1000000000, %rsi
	decq	%rdi
5:	testq	%rdi, %rdi
	js	6f		/* Time is already up.  */

	movq	%rdi, (%rsp)	/* Store relative timeout.  */
	movq	%rsi, 8(%rsp)

	movl	(%r12), %edx
	testl	%edx, %edx
	jz	4f

	movq	%rsp, %r10
	/* XXX The kernel so far uses global futex for the wakeup at
	   all times.  */
#if FUTEX_WAIT == 0
	xorl	%esi, %esi
#else
	movl	$FUTEX_WAIT, %esi
#endif
	movq	%r12, %rdi
	movl	$SYS_futex, %eax
	syscall

	cmpl	$0, (%rdi)
	jne	1f
4:	xorl	%eax, %eax

8:	addq	$16, %rsp
	cfi_adjust_cfa_offset(-16)
	popq	%r13
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r13)
	popq	%r12
	cfi_adjust_cfa_offset(-8)
	cfi_restore(%r12)
	retq

	cfi_adjust_cfa_offset(32)
1:	cmpq	$-ETIMEDOUT, %rax
	jne	2b

6:	movl	$ETIMEDOUT, %eax
	jmp	8b
	cfi_endproc
	.size	__lll_timedwait_tid,.-__lll_timedwait_tid
#endif
