#define IBNIZ_MAIN
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ibniz.h"

struct test_result {
	char *code;
	int result; /* 0: pass, 1: fail */
	char *msg;
	size_t frames;
	char *compile_duration;
	char *run_duration;
};

struct test {
	char *code;
	uint32_t res;
};

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

void timer_start(struct timeval *timer) {
	gettimeofday(&timer[0], NULL);
}

void timer_stop(struct timeval *timer) {
	if (timer == NULL)
		return;
	gettimeofday(&timer[1], NULL);
	if (timeval_diff(&timer[2], &timer[1], &timer[0]))
		printf("Time went backwards!");
}

void output_pretty(int fd, struct test_result *tr) {
}

void output_yaml(int fd, struct test_result *tr) {
}

/* TODO: Use the test_result struct for the result */
int runtest(char *code, uint32_t correct_stacktop)
{
	size_t frame_count = 0;
	struct timeval *compile_time;
	struct timeval *run_time;
	//struct test_result res;
	int err = 0;

	compile_time = calloc(3, sizeof(struct timeval));
	run_time = calloc(3, sizeof(struct timeval));

	timer_start(compile_time);
	vm_init();
	vm_compile(code);
	timer_stop(compile_time);

	timer_start(run_time);
	while (!vm.stopped)
		frame_count += vm_run();
	timer_stop(run_time);

	printf("-\n  code: \"%s\"\n", code);
	puts("  unit_test:");
	if (vm.stack[vm.sp] == 0) {
		err = 1;
		puts("        status: failed");
		puts("        msg: stacktop is 0x0");
	}
	else if (vm.stack[vm.sp] != correct_stacktop) {
		err = 1;
		puts("        status: failed");
		printf("        msg: stacktop=0x%X (should have been 0x%X)\n",
			vm.stack[vm.sp], correct_stacktop);
	}
	else
		puts("        status: passed");

	puts("  stats:");
	printf("    frames: %zu\n    compile: %lu.%uu\n    run: %lu.%uu\n",
		frame_count,
		compile_time[2].tv_sec, compile_time[2].tv_usec,
		run_time[2].tv_sec, run_time[2].tv_usec);

	puts("");
	return err;
}

int main()
{
	/* TODO: i guess we need a little bit more coverage here */
	struct test tests[] = {
		/* Integer tests */
		{"12345T", 0x23450001},
		{"F.1234T", 0xF1234},
		{"123456789ABCDT", 0xABCD6789},
		{"1234.56789AT", 0x9A345678},
		{"1.15.25+T", 0x13A00},

		/* operator tests */
		{"1,1+T", 2 << 16},
		{"6,6*T", 36 << 16},
		{"1,4X3*LT", 81 << 16},
		{"1,2,3,2)T", 1 << 16},
		{"3?5:1;T", 5 << 16},
		{"0?5:1;T", 1 << 16},

		/* Julia test */
		{"2*2!2*3!10rdF2*s0!F9*s1!10,6![2@d3@*4!d*2!3@d*3!3@2@+2@3@-0@+2!4@d+1@+3!4-<6@1-d6!*]6@4r.FF^1977+T", 0x19770f00},
		/* Mandelbrot-zoomer test */
		{"vArs1ldv*vv*0!1-1!0dFX4X1)Lv*vv*-vv2**0@+x1@+4X1)Lv*vv*+4x->?Lpp0:ppRpRE.5*;T", 0x1},
		{NULL, 0},
	};
	size_t numtests = sizeof(tests) / sizeof(struct test);
	size_t err = 0;
	int i;

	for (i = 0; i < numtests && tests[i].code != NULL; i++) {
		err += runtest(tests[i].code, tests[i].res);
	}
	return (err != 0);
}
