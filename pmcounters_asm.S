#include "pmcounters.h"

    .macro val_cmp old, new
    mov \new, %eax
    neg %eax
    mov \old, %ebx
    cmp %ebx, %eax
    cmovb %eax, %ebx
    mov %ebx, \old
    .endm

    .text
    .global pmcounters_rdtsc_ohead
    .type pmcounters_rdtsc_ohead, @function
pmcounters_rdtsc_ohead:
    .cfi_startproc
    push %rbp
    movq %rsp, %rbp
    push %rbx
    push %rcx
    push %rdx

L1:
    rdpmc_start rdpmc_regs, rdpmc_val
    rdtsc_start rdtsc_val
    rdtsc_stop rdtsc_val
    rdpmc_stop rdpmc_regs, rdpmc_val
    
    /*tsc*/
    val_cmp rdtsc_tsc_ohead, rdtsc_val
    /*  ffc 0 */    
    val_cmp rdtsc_pmc_ohead, rdpmc_val
    /*  ffc 1 */
    val_cmp rdtsc_pmc_ohead+0x4, rdpmc_val+0x4
    /* ffc 2 */
    val_cmp rdtsc_pmc_ohead+0x8, rdpmc_val+0x8
    mov rdpmc_val+0x8, %eax
    /* pmc 0 */
    val_cmp rdtsc_pmc_ohead+0xc, rdpmc_val+0xc
    /* pmc 1 */
    val_cmp rdtsc_pmc_ohead+0x10, rdpmc_val+0x10
    /* pmc 2 */
    val_cmp rdtsc_pmc_ohead+0x14, rdpmc_val+0x14
    /* pmc 3 */
    val_cmp rdtsc_pmc_ohead+0x18, rdpmc_val+0x18

    dec %rdi
    jnz L1
    
    pop %rdx
    pop %rcx
    pop %rbx
    movq %rbp, %rsp
    pop %rbp
    repz ret
    .cfi_endproc
    .size pmcounters_rdtsc_ohead, .-pmcounters_rdtsc_ohead
