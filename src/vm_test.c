#define IBNIZ_MAIN
#include <sys/time.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ibniz.h"

int
timeval_diff (struct timeval *result, struct timeval *x, struct timeval *y)
{
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	return x->tv_sec < y->tv_sec;
}

void waitfortimechange()
{
}

uint32_t gettimevalue()
{
	return 0;
}

void runtest(char *code, uint32_t correct_stacktop)
{
	size_t frame_count = 0;
	struct timeval compile_time[3];
	struct timeval run_time[3];

	memset(compile_time, 0, sizeof(compile_time));
	memset(run_time, 0, sizeof(run_time));

	gettimeofday(&compile_time[0], NULL);
	vm_init();
	vm_compile(code);
	gettimeofday(&compile_time[1], NULL);

	gettimeofday(&run_time[0], NULL);
	while (!vm.stopped)
		frame_count += vm_run();
	gettimeofday(&run_time[1], NULL);

	if (timeval_diff(&compile_time[2], &compile_time[1], &compile_time[0]))
		printf("Time went backwards!");
	if (timeval_diff(&run_time[2], &run_time[1], &run_time[0]))
		printf("Time went backwards!");

	if (vm.stack[vm.sp] != correct_stacktop) {
		printf("unit test failed with code \"%s\"\n", code);
		printf("stacktop=%x (should have been %x)\n",
			vm.stack[vm.sp], correct_stacktop);
	}
	else {
		printf(
		    "code: \"%s\"\n"
		    "frames: %zu\n"
		    "compile: %" PRIu32 ".%" PRIu32 "\n"
		    "run: %" PRIu32 ".%" PRIu32 "\n\n",
			code, frame_count,
			(uint32_t)compile_time[2].tv_sec, (uint32_t)compile_time[2].tv_usec,
			(uint32_t)run_time[2].tv_sec, (uint32_t)run_time[2].tv_usec);
	}
}

void test_numbers()
{
	runtest("12345T", 0x23450001);
	runtest("F.1234T", 0xF1234);
	runtest("123456789ABCDT", 0xABCD6789);
	runtest("1234.56789AT", 0x9A345678);
	runtest("1.15.25+T", 0x13A00);
}

int main()
{
	/* TODO: i guess we need a little bit more coverage here */

	test_numbers();
	runtest("1,1+T", 2 << 16);
	runtest("6,6*T", 36 << 16);
	runtest("1,4X3*LT", 81 << 16);
	runtest("1,2,3,2)T", 1 << 16);
	runtest("3?5:1;T", 5 << 16);
	runtest("0?5:1;T", 1 << 16);
	/* Julia test, for run performance */
	runtest("2*2!2*3!10rdF2*s0!F9*s1!10,6![2@d3@*4!d*2!3@d*3!3@2@+2@3@-0@+2!4@d+1@+3!4-<6@1-d6!*]6@4r.FF^1977+T", 0x19770f00);
	return 0;
}
