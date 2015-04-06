#include "test/Test.h"
#include <cstdio>

int main()
{
#ifdef PL_PERFORMANCE_TEST
	Penciloid::PenciloidTest::ConsecutivePerformanceTest("perf_test.txt");
#else
	Penciloid::PenciloidTest::RunTestAll();
#endif

	return 0;
}