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
	field.SetClue(1, 1, MasyuField::CLUE_BLACK);
	field.SetClue(2, 4, MasyuField::CLUE_BLACK);

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
	field.SetClue(0, 1, MasyuField::CLUE_WHITE);
	field.SetClue(0, 3, MasyuField::CLUE_WHITE);

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
	field.SetClue(1, 1, MasyuField::CLUE_BLACK);
	field.SetClue(1, 3, MasyuField::CLUE_WHITE);
	field.SetClue(2, 4, MasyuField::CLUE_BLACK);
	field.SetClue(4, 0, MasyuField::CLUE_BLACK);

	assert(field.GetStatus() == SolverStatus::SUCCESS);
}

void PenciloidTest::MasyuTest4()
{
	MasyuField field;

	field.Init(5, 5);
	field.SetClue(0, 1, MasyuField::CLUE_BLACK);

	field.Init(5, 5);
	assert(field.GetSegmentStyle(1, 2) == MasyuField::LOOP_UNDECIDED);

	field.Debug();
}

}
