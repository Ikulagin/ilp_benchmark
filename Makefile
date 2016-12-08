saxpy_bin = saxpy
pmcounters_objs = pmcounters.o pmcounters_asm.o
saxpy_objs = saxpy_main.o saxpy_sse.o saxpy_sse_ilp.o

mulps_addps_bin = mulps_addps
mulps_addps_objs = mulps_addps_main.o mulps_addps.o
#prog_asms = saxpy_sse.s saxpy_sse_ilp.s

CC := gcc
CFLAGS := -Wall -O0 -g3 -I/home/ikulagin/opt/iaca-lin64/include/
LDFLAGS := -lm

all: $(saxpy_bin) $(mulps_addps_bin)

$(saxpy_bin): $(pmcounters_objs) $(saxpy_objs) $(saxpy_asms)
	$(CC) -o $@ $^ $(LDFLAGS)

$(mulps_addps_bin): $(pmcounters_objs) $(mulps_addps_objs)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $< -o $@

clean:
	rm -f $(saxpy_bin) $(saxpy_objs) $(pmcounters_objs) $(mulps_addps_bin) $(mulps_addps_objs)
