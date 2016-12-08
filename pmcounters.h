#ifndef PMCOUNTERS_H
#define PMCOUNTERS_H

#ifndef __ASSEMBLER__

#define PMC_NAME_LEN 10
#define PMC_MAX 4
#define FFC_MAX 3

enum msr_addresses {
    IA32_PMC0 = 0xc1,
    IA32_PMC1,
    IA32_PMC2,
    IA32_PMC3,
    IA32_PMC4,
    IA32_PMC5,
    IA32_PMC6,
    IA32_PMC7,
    IA32_PERFEVTSEL0 = 0x186,
    IA32_PERFEVTSEL1,
    IA32_PERFEVTSEL2,
    IA32_PERFEVTSEL3,
    IA32_FIXED_CTR_CTRL = 0x38d,
    IA32_PERF_GLOBAL_CTRL = 0x38f,
    IA32_PERF_FIXED_CTR0 = 0x40000000,
    IA32_PERF_FIXED_CTR1 = 0x40000001,
    IA32_PERF_FIXED_CTR2 = 0x40000002,
};

enum perf_event {
#define DEF_PERF_EVENT(ENUM, VAL) ENUM = VAL ,
#include "perf_events.def"
#undef DEF_PERF_EVENT
};

enum perf_event_umask {
#define DEF_PERF_EVENT_UMASK(ENUM, VAL) ENUM = VAL ,
#include "perf_events.def"
#undef DEF_PERF_EVENT_UMASK
};

/* Performance Monitoring Counters description structure */
typedef struct pmc_desc_s {
    char pmc_name[PMC_NAME_LEN];
    enum perf_event event;
    enum perf_event_umask event_mask;
    enum msr_addresses pm_evlist_reg;
    enum msr_addresses pmc_reg;
} pmc_desc_t;

/* Fixed-Function Counters description structure */
typedef struct ffc_desc_s {
    char ffc_name[PMC_NAME_LEN];
    enum msr_addresses ctr_reg;
} ffc_desc_t;

extern int rdpmc_regs[];
extern int rdpmc_val[];
extern int rdtsc_pmc_ohead[];
extern int rdtsc_tsc_ohead;
extern int rdtsc_val;

int pmcounters_init();
int pmcounters_queue(pmc_desc_t *pmc, ffc_desc_t *ffc);
int pmcounters_start();
int pmcounters_stop();
int pmcounters_finalize();

#else /* __ASSEMBLER__ */

.globl rdpmc_regs
.globl rdpmc_val
.globl rdtsc_pmc_ohead
.globl rdtsc_tsc_ohead
.globl rdtsc_val

.macro serialize
    xor %eax, %eax
    cpuid
.endm

.macro rdpmc_start regs, vals
    /* ffc 0*/
    mov \regs,  %ecx          
    rdpmc                     
    mov %eax, \vals           
    /*  ffc 1 */               
    mov \regs+0x4, %ecx    
    rdpmc                     
    mov %eax, \vals+0x4    
    /* ffc 2 */               
    mov \regs+0x8, %ecx    
    rdpmc                     
    mov %eax, \vals+0x8    
    /* pmc 0 */               
    mov \regs+0xc, %ecx    
    rdpmc                    
    mov %eax, \vals+0xc    
    /* pmc 1 */               
    mov \regs+0x10, %ecx   
    rdpmc                    
    mov %eax, \vals+0x10   
    /* pmc 2 */               
    mov \regs+0x14, %ecx   
    rdpmc                    
    mov %eax, \vals+0x14   
    /* pmc 3 */               
    mov \regs+0x18, %ecx   
    rdpmc                    
    mov %eax, \vals+0x18   
.endm

.macro rdpmc_stop regs, vals
    /* ffc 0*/
    mov \regs,  %ecx          
    rdpmc                     
    sub %eax, \vals           
    /*  ffc 1 */               
    mov \regs+0x4, %ecx    
    rdpmc                     
    sub %eax, \vals+0x4    
    /* ffc 2 */               
    mov \regs+0x8, %ecx    
    rdpmc                     
    sub %eax, \vals+0x8    
    /* pmc 0 */               
    mov \regs+0xc, %ecx    
    rdpmc                    
    sub %eax, \vals+0xc    
    /* pmc 1 */               
    mov \regs+0x10, %ecx   
    rdpmc                    
    sub %eax, \vals+0x10   
    /* pmc 2 */               
    mov \regs+0x14, %ecx   
    rdpmc                    
    sub %eax, \vals+0x14   
    /* pmc 3 */               
    mov \regs+0x18, %ecx   
    rdpmc                    
    sub %eax, \vals+0x18   
.endm
    
.macro rdtsc_start tsc_val
    serialize

    rdtsc
    mov %eax, \tsc_val

    serialize
.endm

.macro rdtsc_stop tsc_val
    serialize

    rdtsc
    sub %eax, \tsc_val

    serialize
.endm

.macro overhead val, ohead, save
    mov \val, %eax
    neg %eax
    sub \ohead, %eax
    mov %eax, \save
.endm

.macro pmc_overhead val, ohead, save
    /*  ffc 0 */
    overhead \val, \ohead, \save
    /*  ffc 1 */
    overhead \val+0x4, \ohead+0x4, \save+0x4
    /* ffc 2 */
    overhead \val+0x8, \ohead+0x8, \save+0x8
    /* pmc 0 */
    overhead \val+0xc, \ohead+0xc, \save+0xc
    /* pmc 1 */
    overhead \val+0x10, \ohead+0x10, \save+0x10
    /* pmc 2 */
    overhead \val+0x14, \ohead+0x14, \save+0x14
    /* pmc 3 */
    overhead \val+0x18, \ohead+0x18, \save+0x18
.endm

.macro pmc_start pmc_regs, pmc_vals, tsc_vals
    push %rbx
    push %rcx
    push %rdx

    serialize
    
    rdpmc_start \pmc_regs, \pmc_vals

    rdtsc_start \tsc_vals
 
    pop %rdx
    pop %rcx
    pop %rbx
.endm

.macro pmc_stop pmc_regs, pmc_vals, tsc_val, tsc_ohead, pmc_ohead
    push %rbx
    push %rcx
    push %rdx

    rdtsc_stop \tsc_val

    rdpmc_stop \pmc_regs, \pmc_vals

    serialize

    overhead \tsc_val, \tsc_ohead, \tsc_val
    serialize
    pmc_overhead \pmc_vals, \pmc_ohead, \pmc_vals
    serialize

    pop %rdx
    pop %rcx
    pop %rbx
.endm

#endif /* __ASSEMBLER__ */

#endif //PMCOUNTERS_H
