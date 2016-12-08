#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "MSRDriver.h"
#include "MSRdrvL.h"
#include "pmcounters.h"

#define MSR_DRV "/dev/MSRdrv"

typedef struct msr_queue_s {
    int size;
    struct SMSRInOut q[MAX_QUE_ENTRIES];
} msr_queue_t;

typedef struct pmcounters_s {
    int msrdrv_fd;
    msr_queue_t q_start;
    msr_queue_t q_stop;
} pmcounters_t;

void pmcounters_rdtsc_ohead(int repeat_number);

static int msrqueue_put(msr_queue_t *q, enum EMSR_COMMAND msr_command,
                 unsigned int register_number,
                 unsigned int value_lo,
                 unsigned int value_hi);
static int msrqueue_send(msr_queue_t *msr_q);

int rdpmc_regs[PMC_MAX + FFC_MAX];
int rdpmc_val[PMC_MAX + FFC_MAX];
int rdtsc_pmc_ohead[PMC_MAX + FFC_MAX];
int rdtsc_tsc_ohead;
int rdtsc_val;

static pmcounters_t pmcounters;

int pmcounters_init()
{
    if ((pmcounters.msrdrv_fd = open(MSR_DRV, 0)) == -1) {
        fprintf(stderr, "%s:%d open %s error\n",
                __func__, __LINE__, MSR_DRV);
        return -1;
    }
    
    pmcounters.q_start.size = pmcounters.q_stop.size = 0;

    for (int i = 0; i < MAX_QUE_ENTRIES; i++) {
        pmcounters.q_start.q[i].msr_command = MSR_STOP;
        pmcounters.q_stop.q[i].msr_command = MSR_STOP;
    }

    // enable readpmc instruction
    msrqueue_put(&pmcounters.q_start, PMC_ENABLE, 0, 0, 0);
    // disable readpmc instruction
    msrqueue_put(&pmcounters.q_stop, PMC_DISABLE, 0, 0, 0);
    
    return 0;
}

int pmcounters_queue(pmc_desc_t *pmc, ffc_desc_t *ffc)
{
    int lo = 0, hi = 0;

    // enable counters
    // set enabled PMC
    lo = (1 << PMC_MAX) - 1;
    // set enabled fixed-function counters 
    hi = (1 << FFC_MAX) - 1;
    msrqueue_put(&pmcounters.q_start, MSR_WRITE, IA32_PERF_GLOBAL_CTRL, lo, hi);
    msrqueue_put(&pmcounters.q_stop, MSR_WRITE, IA32_PERF_GLOBAL_CTRL, 0, 0);

    // Setup fixed-function counters
    lo = 0;
    for (int i = 0; i < FFC_MAX; i++) {
        /* See Intel® 64 and IA-32 Architectures Software Developer’s Manual
           Volume 3B: System Programming Guide, Part 2, Chapter 18.2.3 */
        lo |= 2 << (4 * i); 
        /* 0 - disable; 1 - OS; 2 - user; 3 - all rings levels */

        // For rdpmc instruction
        rdpmc_regs[i] = ffc[i].ctr_reg;
    }
    msrqueue_put(&pmcounters.q_start, MSR_WRITE, IA32_FIXED_CTR_CTRL, lo, 0);
    msrqueue_put(&pmcounters.q_stop, MSR_WRITE, IA32_FIXED_CTR_CTRL, 0, 0);


    // Setup PMC
    for (int i = 0; i < PMC_MAX; i++) {
        lo = 0;
        lo = (1 << 22) | (1 << 16) | (pmc[i].event_mask << 8) | pmc[i].event;
        msrqueue_put(&pmcounters.q_start, MSR_WRITE, pmc[i].pm_evlist_reg,
                     lo, 0);
        msrqueue_put(&pmcounters.q_stop, MSR_WRITE, pmc[i].pm_evlist_reg,
                     0, 0);
        msrqueue_put(&pmcounters.q_start, MSR_WRITE, pmc[i].pmc_reg,
                     0, 0);
        msrqueue_put(&pmcounters.q_stop, MSR_WRITE, pmc[i].pmc_reg,
                     0, 0);
        // For rdpmc instruction
        rdpmc_regs[FFC_MAX + i] = i;
    }

    return 0;
}

int pmcounters_start()
{
    rdtsc_tsc_ohead = -1;
    for (int i = 0; i < FFC_MAX + PMC_MAX; i++)
        rdtsc_pmc_ohead[i] = -1;
    msrqueue_send(&pmcounters.q_start);
    pmcounters_rdtsc_ohead(5);

    return 0;
}

int pmcounters_stop()
{
    msrqueue_send(&pmcounters.q_stop);

    return 0;
}

int pmcounters_finalize()
{
    close(pmcounters.msrdrv_fd);

    return 0;
}

// msrqueue functions
static int msrqueue_put(msr_queue_t *msr_q, enum EMSR_COMMAND msr_command,
                 unsigned int register_number,
                 unsigned int value_lo,
                 unsigned int value_hi)
{
    if (msr_q->size >= MAX_QUE_ENTRIES)
        return -1;
    
    msr_q->q[msr_q->size].msr_command = msr_command;
    msr_q->q[msr_q->size].register_number = register_number;
    msr_q->q[msr_q->size].val[0] = value_lo;
    msr_q->q[msr_q->size].val[1] = value_hi;
    msr_q->size++;

    return 0;
}

static int msrqueue_send(msr_queue_t *msr_q)
{
    ioctl(pmcounters.msrdrv_fd, IOCTL_PROCESS_LIST, msr_q->q);
    return 0;
}
