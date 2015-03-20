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
	prob.SetHint(0, 1, 17, KakuroProblem::HINT_NONE);
	prob.SetHint(0, 2, 11, KakuroProblem::HINT_NONE);
	prob.SetHint(1, 0, KakuroProblem::HINT_NONE, 16);
	prob.SetHint(2, 0, KakuroProblem::HINT_NONE, 12);

	KakuroField field;
	field.Init(prob);
	field.CheckAll();

	field.Debug();
	for (int i = 1; i < 3; ++i) {
		for (int j = 1; j < 3; ++j) printf("%d ", field.GetCellValue(i, j));
		printf("\n");
	}
}

}
