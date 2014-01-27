#define IBNIZ_MAIN
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ibniz.h"

struct tv_avg {
	struct timeval high;
	struct timeval low;
	struct timeval total;
};

struct test_result {
	struct tv_avg compile_tv;
	uint32_t compile_iterations;
	struct tv_avg run_tv;
	uint32_t run_iterations;
	uint32_t frames;
	uint8_t result;
};

struct test {
	char *code;
	int32_t stacktop_expected;
	int32_t stacktop_actual;
	struct test_result results;
};

void test_init(struct test *test, uint32_t iterations) {
	memset(&test->results.compile_tv, 0, sizeof(struct tv_avg));
	memset(&test->results.run_tv, 0, sizeof(struct tv_avg));
	test->results.compile_iterations = 0;
	test->results.run_iterations = 0;
	test->results.frames = 0;
	test->results.result = -1;
	test->results.compile_iterations = 0;
	test->results.run_iterations = 0;
}

void waitfortimechange()
{
}

uint32_t gettimevalue()
{
	return 0;
}

void timer_start(struct timeval *timer) {
	if (timer == NULL)
		return;
	gettimeofday(&timer[0], NULL);
}

void timer_stop(struct timeval *timer) {
	if (timer == NULL)
		return;
	gettimeofday(&timer[1], NULL);
	timersub(&timer[1], &timer[0], &timer[2]);
}

void output_pretty(int fd, struct test *test) {
}

void output_yaml(FILE *fd, struct test *test) {
	uint8_t err = 0;
	fprintf(fd, "-\n  code: \"%s\"\n", test->code);
	fputs("  unit_test:\n", fd);
	if (test->stacktop_actual == 0) {
		err = 1;
		fputs("        status: failed\n", fd);
		fputs("        msg: stacktop is 0x0\n", fd);
	}
	else if (test->stacktop_actual != test->stacktop_expected) {
		err = 1;
		fputs("        status: failed\n", fd);
		fprintf(fd, "        msg: stacktop=0x%X (should have been 0x%X)\n",
			test->stacktop_actual, test->stacktop_expected);
	}
	else
		fputs("        status: passed\n", fd);

	fputs("  stats:\n", fd);
	fprintf(fd,
	    "    compile:\n"
	    "      iterations: %" PRIu32 "\n"
	    "      high: %" PRIu32 ".%" PRIu32 "\n"
	    "      low: %" PRIu32 ".%" PRIu32 "\n"
	    "      total: %" PRIu32 ".%" PRIu32 "\n",
			test->results.compile_iterations,
			(uint32_t)test->results.compile_tv.high.tv_sec,
			(uint32_t)test->results.compile_tv.high.tv_usec,
			(uint32_t)test->results.compile_tv.low.tv_sec,
			(uint32_t)test->results.compile_tv.low.tv_usec,
			(uint32_t)test->results.compile_tv.total.tv_sec,
			(uint32_t)test->results.compile_tv.total.tv_usec);
	fprintf(fd,
	    "    run:\n"
	    "      iterations: %" PRIu32 "\n"
	    "      frames: %" PRIu32 "\n"
	    "      high: %" PRIu32 ".%" PRIu32 "\n"
	    "      low: %" PRIu32 ".%" PRIu32 "\n"
	    "      total: %" PRIu32 ".%" PRIu32 "\n",
			test->results.run_iterations,
			test->results.frames,
			(uint32_t)test->results.run_tv.high.tv_sec,
			(uint32_t)test->results.run_tv.high.tv_usec,
			(uint32_t)test->results.run_tv.low.tv_sec,
			(uint32_t)test->results.run_tv.low.tv_usec,
			(uint32_t)test->results.run_tv.total.tv_sec,
			(uint32_t)test->results.run_tv.total.tv_usec);

	fputs("\n", fd);
}

void timer_result(struct timeval *tv, struct tv_avg *y) {
	if (timercmp(tv, &y->high, >))
		y->high = *tv;
	else if (timercmp(tv, &y->low, <))
		y->low = *tv;
	timeradd(&y->total, tv, &y->total);
}

/* TODO: Use the test_result struct for the result */
int runtest(struct test *test) {
	size_t frame_count;
	struct timeval *run_time;
	//struct test_result res;
	int err = 0;
	uint32_t iter, repeat = 100;
	test_init(test, repeat);
	run_time = calloc(3, sizeof(struct timeval));

	vm_init();
	for (iter=0; iter < repeat; iter++) {
		timer_start(run_time);
		vm_compile(test->code);
		timer_stop(run_time);
		timer_result(&run_time[2], &test->results.compile_tv);
	}
	test->results.compile_iterations = iter;

	for (iter=0; iter < repeat; iter++) {
		frame_count = 0;
		timer_start(run_time);
		while (vm.stopped == 0)
			frame_count += vm_run();
		timer_stop(run_time);
		/* only testing the first run */
		if (iter == 0) {
			test->results.frames = frame_count;
			test->stacktop_actual = vm.stack[vm.sp];
			if (test->stacktop_actual == 0 ||
			    test->stacktop_expected != test->stacktop_actual) {
				test->results.result = 1;
				break;
			}
		}
		vm.stopped = 0;
		timer_result(&run_time[2], &test->results.run_tv);
	}
	test->results.run_iterations = iter;

	output_yaml(stdout, test);
	free(run_time);
	return err;
}

int main() {
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
		{NULL},
	};
	size_t numtests = sizeof(tests) / sizeof(struct test);
	size_t err = 0;
	int i;

	for (i = 0; i < numtests && tests[i].code != NULL; i++) {
		err += runtest(&tests[i]);
	}
	return (err != 0);
}
