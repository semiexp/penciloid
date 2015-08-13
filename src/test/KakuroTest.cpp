#include <cassert>
#include <cstdio>

#include "../kakuro/KakuroField.h"
#include "../kakuro/KakuroProblem.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::KakuroTest1()
{
	KakuroProblem prob;

	prob.Init(3, 3);
	prob.SetNumberCell(1, 1);
	prob.SetNumberCell(1, 2);
	prob.SetNumberCell(2, 1);
	prob.SetNumberCell(2, 2);
	prob.SetClue(0, 1, 17, KakuroProblem::CLUE_NONE);
	prob.SetClue(0, 2, 11, KakuroProblem::CLUE_NONE);
	prob.SetClue(1, 0, KakuroProblem::CLUE_NONE, 16);
	prob.SetClue(2, 0, KakuroProblem::CLUE_NONE, 12);

	KakuroField field;
	field.Init(prob);
	field.CheckAll();

	field.Debug();
}

}
