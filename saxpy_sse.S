#include "pmcounters.h"
    
    .text
    .globl	saxpy_sse
	.type	saxpy_sse, @function
saxpy_sse:
	.cfi_startproc
    leal	3(%rdx), %eax
	testl	%edx, %edx
	cmovs	%eax, %edx
	sarl	$2, %edx
	shufps	$0, %xmm0, %xmm0
	testl	%edx, %edx
	jle	.L5

    # start get PMC
    pmc_start rdpmc_regs, rdpmc_val, rdtsc_val
    # finish

	leal	-1(%rdx), %edx
	addq	$1, %rdx
	salq	$4, %rdx
	movq	$0, %rax
.L7:
/*	movaps	%xmm0, %xmm1
    movaps (%rdi,%rax), %xmm2
    movaps	(%rsi,%rax), %xmm3
	mulps	%xmm2, %xmm1
	addps	%xmm3, %xmm1
	movaps	%xmm1, (%rsi,%rax)
*/
    movaps	%xmm0, %xmm1
    mulps	(%rdi,%rax), %xmm1
	addps	(%rsi,%rax), %xmm1
	movaps	%xmm1, (%rsi,%rax)
    movaps	%xmm0, %xmm1
    mulps	16(%rdi,%rax), %xmm1
	addps	16(%rsi,%rax), %xmm1
	movaps	%xmm1, 16(%rsi,%rax)
	addq	$32, %rax
	cmpq	%rdx, %rax
	jne	.L7
.L5:
    /* start get PMC */
    pmc_stop rdpmc_regs, rdpmc_val, rdtsc_val, rdtsc_tsc_ohead, rdtsc_pmc_ohead
    /* finish */
    rep ret
	.cfi_endproc
	.size	saxpy_sse, .-saxpy_sse

#   movaps	%xmm0, %xmm1
#   mulps	(%rdi,%rax), %xmm1
#	addps	(%rsi,%rax), %xmm1
#	movaps	%xmm1, (%rsi,%rax)
