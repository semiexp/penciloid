#include <cassert>

#include "../masyu/MasyuField.h"
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

}
