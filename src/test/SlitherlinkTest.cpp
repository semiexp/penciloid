#include <cassert>
#include <ctime>

#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

namespace Penciloid
{
bool PenciloidTest::SlitherlinkCheckGrid(SlitherlinkField &field, const int *expected)
{
	int height = field.GetHeight() * 2 + 1, width = field.GetWidth() * 2 + 1;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (i % 2 != j % 2) {
				int expected_style = (expected[i * width + j] == 1) ? SlitherlinkField::LOOP_LINE : SlitherlinkField::LOOP_BLANK;
				if (field.GetSegmentStyle(i, j) != expected_style) return false;
			}
		}
	}

	return true;
}

void PenciloidTest::SlitherlinkTest1()
{
	SlitherlinkField field;
	static const int expected[] = {
		0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1,
		0, 0, 0, 1, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 1,
		0, 1, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 1,
		0, 1, 0, 1, 0, 1, 0,
	};

	field.Init(3, 3);
	
	field.SetHint(0, 0, 0);
	field.SetHint(0, 1, 2);
	field.SetHint(2, 0, 3);
	field.SetHint(2, 1, 1);

	assert(field.GetStatus() == SolverStatus::SUCCESS);
	assert(SlitherlinkCheckGrid(field, expected));
}

void PenciloidTest::SlitherlinkTest2()
{
	SlitherlinkField field;
	static const int expected[] = {
		0, 1, 0, 1, 0, 1, 0,
		1, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 1, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 1,
		0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 1,
		0, 0, 0, 0, 0, 1, 0,
	};

	field.Init(3, 3);

	field.SetHint(1, 0, 3);
	field.SetHint(1, 1, 3);
	field.SetHint(2, 2, 3);

	assert(field.GetStatus() == SolverStatus::SUCCESS);
	assert(SlitherlinkCheckGrid(field, expected));
}

void PenciloidTest::SlitherlinkTest3()
{
	SlitherlinkDatabase::CreateDatabase();

	for (int i = 1; i <= NUMBER_OF_SLITHERLINK_PROBLEM; ++i) {
		SlitherlinkProblem prob;

		SlitherlinkLoadProblem(prob, i);

		SlitherlinkField field;
		field.Init(prob);

		field.CheckAll();
		field.Assume();

		if (field.GetStatus() == SolverStatus::SUCCESS) {
			printf("Slitherlink problem test #%d: OK\n", i);
		} else {
			printf("Slitherlink problem test #%d: FAIL (status: %d)\n", i, field.GetStatus());
			field.Debug();
		}
	}
}

void PenciloidTest::SlitherlinkPerformanceTest()
{
	SlitherlinkDatabase::CreateDatabase();

	clock_t start, end;
	const int num_trial = 100;

	start = clock();
	int successful_trial = 0;

	for (int i = 0; i < num_trial; ++i) {
		SlitherlinkProblem prob;

		SlitherlinkLoadProblem(prob, NUMBER_OF_SLITHERLINK_PROBLEM);

		SlitherlinkField field;
		field.Init(prob);

		field.CheckAll();
		field.Assume();
		
		if (field.GetStatus() == SolverStatus::SUCCESS) ++successful_trial;
	}

	end = clock();

	if (successful_trial != num_trial) {
		printf("Something is wrong; only %d out of %d succeeded", successful_trial, num_trial);
	}
	printf("Slitherlink performance test: %f[s] (%d times)\n", (double)(end - start) / CLOCKS_PER_SEC, num_trial);
}

}