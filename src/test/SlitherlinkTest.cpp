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
	
	field.SetClue(0, 0, 0);
	field.SetClue(0, 1, 2);
	field.SetClue(2, 0, 3);
	field.SetClue(2, 1, 1);

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

	field.SetClue(1, 0, 3);
	field.SetClue(1, 1, 3);
	field.SetClue(2, 2, 3);

	assert(field.GetStatus() == SolverStatus::SUCCESS);
	assert(SlitherlinkCheckGrid(field, expected));
}

void PenciloidTest::SlitherlinkTest3()
{
	{
		SlitherlinkProblem prob;
		prob.Init(5, 3);

		prob.SetClue(0, 0, 3);
		prob.SetClue(0, 1, 3);
		prob.SetClue(0, 2, 3);
		prob.SetClue(1, 0, 2);
		prob.SetClue(1, 1, 2);
		prob.SetClue(1, 2, 2);
		prob.SetClue(2, 2, 1);

		SlitherlinkField field;
		field.Init(prob);

		assert(field.GetStatus() == SolverStatus::SUCCESS);
	}

	{
		SlitherlinkProblem prob;
		prob.Init(5, 3);

		prob.SetClue(0, 0, 3);
		prob.SetClue(0, 1, 3);
		prob.SetClue(0, 2, 3);
		prob.SetClue(1, 0, 2);
		prob.SetClue(1, 1, 2);
		prob.SetClue(1, 2, 2);
		prob.SetClue(3, 2, 2);

		SlitherlinkField field;
		field.Init(prob);

		assert(field.GetStatus() & SolverStatus::INCONSISTENT);
	}

}

void PenciloidTest::SlitherlinkTest4()
{
	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);

		prob.SetClue(0, 0, 0);
		prob.SetClue(1, 1, 2);
		prob.SetClue(2, 2, 1);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(4, 5) == SlitherlinkField::LOOP_BLANK);
		assert(field.GetSegmentStyle(5, 4) == SlitherlinkField::LOOP_BLANK);
	}
	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);

		prob.SetClue(0, 0, 1);
		prob.SetClue(1, 1, 2);
		prob.SetClue(2, 2, 1);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(6, 5) == SlitherlinkField::LOOP_BLANK);
		assert(field.GetSegmentStyle(5, 6) == SlitherlinkField::LOOP_BLANK);
	}
	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);

		prob.SetClue(0, 0, 0);
		prob.SetClue(1, 1, 2);
		prob.SetClue(2, 2, 3);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(4, 5) == SlitherlinkField::LOOP_LINE);
		assert(field.GetSegmentStyle(5, 4) == SlitherlinkField::LOOP_LINE);
	}
	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);

		prob.SetClue(0, 0, 1);
		prob.SetClue(1, 1, 2);
		prob.SetClue(2, 2, 3);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(6, 5) == SlitherlinkField::LOOP_LINE);
		assert(field.GetSegmentStyle(5, 6) == SlitherlinkField::LOOP_LINE);
	}
}

}