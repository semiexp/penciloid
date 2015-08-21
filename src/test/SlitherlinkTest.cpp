#include <cassert>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkDatabaseMethod.hpp"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkClueTest(int height, int width, std::vector<const char*> board, int expected_status, int test_id = -1)
{
	SlitherlinkField field;
	field.Init(height, width);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			char clue = board[y * 2 + 1][x * 2 + 1];
			if ('0' <= clue && clue <= '3') field.SetClue(y, x, clue - '0');
		}
	}

	field.CheckAll();

	for (int y = 0; y <= height * 2; ++y) {
		for (int x = 0; x <= width * 2; ++x) {
			if (y % 2 == x % 2) continue; // this is not a segment

			int expected =
				board[y][x] == ' ' ? SlitherlinkField::LOOP_UNDECIDED :
				board[y][x] == 'x' ? SlitherlinkField::LOOP_BLANK : SlitherlinkField::LOOP_LINE;

			if (field.GetSegmentStyle(y, x) != expected) {
				std::cerr << "SlitherlinkClueTest failed in test #" << test_id << std::endl;
				std::cerr << "expected: " << expected << ", actual: " << field.GetSegmentStyle(y, x) << " at (" << y << "," << x << ")" << std::endl;
				exit(1);
			}
		}
	}
}

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

void PenciloidTest::SlitherlinkTest5()
{
	SlitherlinkDatabase::CreateReducedDatabase(SlitherlinkDatabaseMethod());

	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);
		prob.SetClue(0, 0, 1);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(0, 1) == SlitherlinkField::LOOP_BLANK);
		assert(field.GetSegmentStyle(1, 0) == SlitherlinkField::LOOP_BLANK);
	}

	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);
		prob.SetClue(0, 0, 3);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(0, 1) == SlitherlinkField::LOOP_LINE);
		assert(field.GetSegmentStyle(1, 0) == SlitherlinkField::LOOP_LINE);
	}

	{
		SlitherlinkProblem prob;
		prob.Init(4, 4);
		prob.SetClue(0, 0, 2);

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		assert(field.GetStatus() == SolverStatus::NORMAL);
		assert(field.GetSegmentStyle(0, 3) == SlitherlinkField::LOOP_LINE);
		assert(field.GetSegmentStyle(3, 0) == SlitherlinkField::LOOP_LINE);
	}

	SlitherlinkDatabase::ReleaseDatabase();
}

}