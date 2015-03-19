#include <cassert>
#include <ctime>

#include "../masyu/MasyuField.h"
#include "../masyu/MasyuProblem.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::MasyuTest1()
{
	MasyuField field;

	field.Init(5, 5);
	field.SetHint(1, 1, MasyuField::HINT_BLACK);
	field.SetHint(2, 4, MasyuField::HINT_BLACK);

	assert(field.GetSegmentStyle(2, 3) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(2, 5) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(3, 2) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(5, 2) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(4, 5) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(4, 7) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(5, 4) == MasyuField::LOOP_LINE);
	assert(field.GetStatus() == SolverStatus::NORMAL);
}

void PenciloidTest::MasyuTest2()
{
	MasyuField field;

	field.Init(3, 6);
	field.SetHint(0, 1, MasyuField::HINT_WHITE);
	field.SetHint(0, 3, MasyuField::HINT_WHITE);

	assert(field.GetSegmentStyle(0, 1) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(0, 3) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(0, 5) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(0, 7) == MasyuField::LOOP_LINE);
	assert(field.GetSegmentStyle(0, 9) == MasyuField::LOOP_BLANK);
	assert(field.GetStatus() == SolverStatus::NORMAL);
}

void PenciloidTest::MasyuTest3()
{
	MasyuField field;

	field.Init(5, 5);
	field.SetHint(1, 1, MasyuField::HINT_BLACK);
	field.SetHint(1, 3, MasyuField::HINT_WHITE);
	field.SetHint(2, 4, MasyuField::HINT_BLACK);
	field.SetHint(4, 0, MasyuField::HINT_BLACK);

	assert(field.GetStatus() == SolverStatus::SUCCESS);
}

void PenciloidTest::MasyuTest4()
{
	for (int i = 1; i <= NUMBER_OF_MASYU_PROBLEM; ++i) {
		MasyuProblem prob;

		MasyuLoadProblem(prob, i);

		MasyuField field;
		field.Init(prob);

		field.CheckAll();
		field.Assume();

		if (field.GetStatus() == SolverStatus::SUCCESS) {
			printf("Masyu problem test #%d: OK\n", i);
		} else {
			printf("Masyu problem test #%d: FAIL (status: %d)\n", i, field.GetStatus());
			field.Debug();
		}
	}
}

void PenciloidTest::MasyuPerformanceTest()
{
	clock_t start, end;
	const int num_trial = 100;

	start = clock();
	int successful_trial = 0;

	for (int i = 0; i < num_trial; ++i) {
		MasyuProblem prob;

		MasyuLoadProblem(prob, NUMBER_OF_MASYU_PROBLEM);

		MasyuField field;
		field.Init(prob);

		field.CheckAll();
		field.Assume();

		if (field.GetStatus() == SolverStatus::SUCCESS) ++successful_trial;
	}

	end = clock();

	if (successful_trial != num_trial) {
		printf("Something is wrong; only %d out of %d succeeded", successful_trial, num_trial);
	}
	printf("Masyu performance test: %f[s] (%d times)\n", (double)(end - start) / CLOCKS_PER_SEC, num_trial);
}

}
