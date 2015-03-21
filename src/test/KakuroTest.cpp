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
}

void PenciloidTest::KakuroTest2()
{
	for (int i = 1; i <= NUMBER_OF_KAKURO_PROBLEM; ++i) {
		KakuroProblem prob;

		KakuroLoadProblem(prob, i);

		KakuroField field;
		field.Init(prob);

		field.CheckAll();

		field.Debug();
		if (field.GetStatus() == SolverStatus::SUCCESS) {
			printf("Kakuro problem test #%d: OK\n", i);
		} else {
			printf("Kakuro problem test #%d: FAIL (status: %d)\n", i, field.GetStatus());
		}
	}
}

}
