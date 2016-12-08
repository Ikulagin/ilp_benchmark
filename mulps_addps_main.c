#define _GNU_SOURCE
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <xmmintrin.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "pmcounters.h"

#define EPS 1E-6

enum {
    repeat_number = 1000,
    pmc_count = 4,
    n = 100000,
};

typedef void (*test_fnc)(float);
typedef void (*runner_fnc)(int);

void mulps_addps_hazard(float init);
void mulps_addps_nohazard(float init);

typedef struct test_conf_s {
    test_fnc test;
    runner_fnc runner;
} test_conf_t;

test_conf_t test_conf;

pmc_desc_t pmc_description[PMC_MAX] =
    {
        /* conf1
        {"uop p0", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT0,
         IA32_PERFEVTSEL0, IA32_PMC0},
        {"uop p1", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT1,
         IA32_PERFEVTSEL1, IA32_PMC1},
        {"uop p2", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT2,
         IA32_PERFEVTSEL2, IA32_PMC2},
        {"uop p3", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT3,
         IA32_PERFEVTSEL3, IA32_PMC3}
        */
        {"Uops", UOPS_RETIRED, UOPS_RETIRED_ALL,
         IA32_PERFEVTSEL0, IA32_PMC0},
        {"Uop p0", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT0,
         IA32_PERFEVTSEL1, IA32_PMC1},
        {"Uop p1", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT1,
         IA32_PERFEVTSEL2, IA32_PMC2},
        {"Uop p4", UOPS_DISPATCHED_PORT, UOPS_DISPATCHED_PORT4,
         IA32_PERFEVTSEL3, IA32_PMC3}
    };

ffc_desc_t ffc_description[FFC_MAX] =
    {
        {"core cyc", IA32_PERF_FIXED_CTR1},
        {"ref cyc", IA32_PERF_FIXED_CTR2},
        {"instruct", IA32_PERF_FIXED_CTR0}
    };

int counts_val[repeat_number][FFC_MAX + PMC_MAX + 1]; /*0 - tsc*/

static inline pid_t gettid(void) { return syscall(__NR_gettid); }

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

void test_runner(int num_repeat)
{
    pmcounters_start();
    sched_yield();
    double t = wtime();
    test_conf.test(1.0);
    //    saxpy(x, y, a, n);
    t = wtime() - t;    
    sched_yield();
    pmcounters_stop();

    counts_val[num_repeat][0] = rdtsc_val;
    for (int i = 0; i < FFC_MAX + PMC_MAX; i++) {
        counts_val[num_repeat][i + 1] = rdpmc_val[i];
    }


}

int cmp_pmccounts(const void *a, const void *b)
{
    const int *array_a = (const int *)a;
    const int *array_b = (const int *)b;
    return (array_a[0] > array_b[0]) - (array_a[0] < array_b[0]);
}

void argv_process(int argc, char **argv) {

    if (!strcmp("--hazard", argv[1])) {
        printf("run hazard version\n");
        test_conf.test = mulps_addps_hazard;
        test_conf.runner = test_runner;
        return ;
    }

    if (!strcmp("--no-hazard", argv[1])) {
        printf("run no hazard version\n");
        test_conf.test = mulps_addps_nohazard;
        test_conf.runner = test_runner;
        return ;
    }

    fprintf(stderr,
            "%s:%d unknown argument [%s]\n"
            "Expected argument {--hazard, --no-hazard}\n",
            __func__, __LINE__, argv[1]);
    exit(EXIT_FAILURE);
    
}

int main(int argc, char **argv)
{
    int e, p = 2;
    cpu_set_t mask;
   
    if (argc < 2) {
        fprintf(stderr,
                "%s:%d Expected argument {--hazard, --no-hazard}\n",
                __func__, __LINE__);

        exit(EXIT_FAILURE);
    }

    argv_process(argc, argv);

    CPU_ZERO(&mask);
    e = sched_getaffinity(0, sizeof(cpu_set_t), &mask);
    if (e) printf("\nsched_getaffinity failed");
    
    CPU_ZERO(&mask);
    CPU_SET(p, &mask);
    sched_setaffinity(gettid(), sizeof(cpu_set_t), &mask);
    if (e) printf("\nFailed to lock thread to processor %i\n", p); 

    setpriority(PRIO_PROCESS, 0, PRIO_MIN); 

    if (pmcounters_init()) {
        exit(EXIT_FAILURE);
    }
    pmcounters_queue(pmc_description, ffc_description);

    for (int i = 0; i < repeat_number; i++) {
        test_conf.runner(i);
    }
    qsort(counts_val, repeat_number, sizeof(counts_val[0]),
          cmp_pmccounts);
    /* t = t / (double)repeat_number; */
    /* printf("Elapsed time (scalar): [%.6f %.6f %.6f] sec.\n", */
    /*        tmin, t, tmax); */

    printf("%10s", "TSC");
    for (int i = 0; i < FFC_MAX; i++) {
        printf("%10s", ffc_description[i].ffc_name);
    }
    for (int i = 0; i < PMC_MAX; i++) {
        printf("%10s", pmc_description[i].pmc_name);
    }

    printf("\n");

    /*
      unbiased sample variance - Var
     */

    long double tsc_mean = 0;
    for (int i = (repeat_number * 25)/100;
         i < repeat_number - (repeat_number * 25)/100; i++) {
        tsc_mean += (long double)counts_val[i][0];
        for (int j = 0; j < FFC_MAX + PMC_MAX + 1; j++)
            printf("%10d", counts_val[i][j]);
        printf("\n");
    }

    /* for (int i = (repeat_number * 25)/100; */
    /*      i < repeat_number - (repeat_number * 25)/100; i++) { */
    tsc_mean = tsc_mean / (double)(repeat_number - (repeat_number * 25)/100 - (repeat_number * 25)/100);
    printf("tsc_mean_sse_ilp = %d\n", (int)tsc_mean);

    pmcounters_finalize();
        
    return 0;
}
