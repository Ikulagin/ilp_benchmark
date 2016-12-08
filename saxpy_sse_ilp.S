#include "pmcounters.h"

    .text
	.globl	saxpy_sse_ilp
	.type	saxpy_sse_ilp, @function
saxpy_sse_ilp:
    .cfi_startproc
	leal	3(%rdx), %eax
	testl	%edx, %edx
	cmovs	%eax, %edx
	sarl	$2, %edx
	shufps	$0, %xmm0, %xmm0
	testl	%edx, %edx
	jle	.L1

    /* start get PMC */
    pmc_start rdpmc_regs, rdpmc_val, rdtsc_val
    /* finish */
     
	leal	-1(%rdx), %edx
	addq	$1, %rdx
	salq	$4, %rdx
	movl	$0, %eax
    sub $32, %rdx

    /* rsi - yy */
    /* rdi - xx */
    movaps (%rdi), %xmm2 /* xx[0] a0*/
    movaps 16(%rdi), %xmm1 /* xx[16] a1 */
    movaps (%rsi), %xmm3 /* yy[0] b0 */
    mulps %xmm0, %xmm2  /*a0 = C * a0 */
.L3:
    mulps %xmm0, %xmm1 /* a1 = C * a1 */
    addps %xmm2, %xmm3 /* b0 = a0 + b0 */

    movaps 16(%rsi, %rax), %xmm4 /* load b1 */
    movaps 32(%rdi, %rax), %xmm2 /* load a2 */
    movaps %xmm3, (%rsi, %rax) /* store b0 */
    addps %xmm1, %xmm4 /* b1 = a1 + b1 */
    mulps %xmm0, %xmm2 /* a2 = C * a2 */

    movaps 48(%rdi, %rax), %xmm1 /* load a3 */
    movaps 32(%rsi, %rax), %xmm3 /* load b2 */
    movaps %xmm4, 16(%rsi, %rax) /* store b1 */
    
    addq	$32, %rax
    cmpq	%rdx, %rax
    jne	.L3

    mulps %xmm0, %xmm1 /* a3 = C * a1 */
    addps %xmm2, %xmm3 /* b2 = a2 + b2 */
    movaps %xmm3, (%rsi, %rax) /* store b2 */
    movaps 16(%rsi, %rax), %xmm4 /* load b3 */
    addps %xmm1, %xmm4 /* b2 = a2 + b2 */
    movaps %xmm4, 16(%rsi, %rax) /* store b3 */

.L1:

    /* start get PMC */
    pmc_stop rdpmc_regs, rdpmc_val, rdtsc_val, rdtsc_tsc_ohead, rdtsc_pmc_ohead
    /* finish */
    
    ret
	.cfi_endproc
	.size	saxpy_sse_ilp, .-saxpy_sse_ilp
	.ident	"GCC: (GNU) 6.2.1 20160916 (Red Hat 6.2.1-2)"
	.section	.note.GNU-stack,"",@progbits

    /* version 1
    
    movaps %xmm0, %xmm1
    mulps  (%rdi,%rax), %xmm1
    addps  (%rsi,%rax), %xmm1
    movaps %xmm1, (%rsi,%rax)
    */

/* version 2
     leal	-1(%rdx), %edx
 	addq	$1, %rdx
 	salq	$4, %rdx
 	movl	$0, %eax
     subq    $16, %rdx
     # rsi - yy
    # rdi - xx
    movaps 0x10(%rdi, %rax), %xmm1
    movaps (%rdi, %rax), %xmm2
    movaps (%rsi, %rax), %xmm3
    mulps %xmm0, %xmm2
    jmp .L4
.L3:
IACA_START
    .byte 0x0F, 0x0B
    movl $111, %ebx
    .byte 0x64, 0x67, 0x90
    movaps 0x10(%rdi, %rax), %xmm1
    movaps (%rsi, %rax), %xmm3
.L4:
    mulps	%xmm0, %xmm1
    addps	%xmm2, %xmm3
    movaps %xmm1, %xmm2
    movaps	%xmm3, (%rsi,%rax)

    addq	$16, %rax
    cmpq	%rdx, %rax
    jne	.L3
IACA_END
   movl $222, %ebx
   .byte 0x64, 0x67, 0x90
   .byte 0x0F, 0x0B

    movaps (%rsi, %rax), %xmm3
    addps	%xmm2, %xmm3
	movaps	%xmm3, (%rsi,%rax)
.L1:
*/
