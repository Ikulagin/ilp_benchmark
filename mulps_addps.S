#include "pmcounters.h"
    
    .text
    .global mulps_addps_hazard
    .type mulps_addps_hazard, @function
mulps_addps_hazard: /*arg0 = xmm0*/
    .cfi_startproc
    push %rbp
    movq %rsp, %rbp

    shufps $0, %xmm0, %xmm0
    movaps %xmm0, %xmm1
        
    # start get PMC
    pmc_start rdpmc_regs, rdpmc_val, rdtsc_val
    # finish
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    mulps %xmm1, %xmm0
    addps %xmm0, %xmm1
    /* start get PMC */
    pmc_stop rdpmc_regs, rdpmc_val, rdtsc_val, rdtsc_tsc_ohead, rdtsc_pmc_ohead
    /* finish */

    movq %rbp, %rsp
    pop %rbp
    ret
    .cfi_endproc
    .size mulps_addps_hazard, .-mulps_addps_hazard

    .text
    .global mulps_addps_nohazard
    .type mulps_addps_nohazard, @function
mulps_addps_nohazard: /*arg0 = xmm0*/
    .cfi_startproc
    push %rbp
    movq %rsp, %rbp

    shufps $0, %xmm0, %xmm0
    movaps %xmm0, %xmm1

    /* start get PMC */
    pmc_start rdpmc_regs, rdpmc_val, rdtsc_val
    /* finish */
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    mulps %xmm0, %xmm0
    addps %xmm1, %xmm1
    /* start get PMC */
    pmc_stop rdpmc_regs, rdpmc_val, rdtsc_val, rdtsc_tsc_ohead, rdtsc_pmc_ohead
    /* finish */

    
    movq %rbp, %rsp
    pop %rbp
    ret
    .cfi_endproc
    .size mulps_addps_nohazard, .-mulps_addps_nohazard
