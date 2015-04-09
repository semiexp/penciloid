#include "test/Test.h"
#include <cstdio>

int main()
{
#ifdef PL_SL_GENERATOR_TEST
	Penciloid::PenciloidTest::SlitherlinkGeneratorTest(36, 20);
#else
#ifdef PL_PERFORMANCE_TEST
	Penciloid::PenciloidTest::ConsecutivePerformanceTest("perf_test.txt");
#else
	Penciloid::PenciloidTest::RunTestAll();
#endif
#endif

	return 0;
}