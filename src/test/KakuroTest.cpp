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
	KakuroProblem prob;

	// Source: http://www.nikoli.com/ja/puzzles/kakuro/ Sample problem 1
	int problem_array[] = {
		-1, -1, -1, -1, 16, -1, 4, -1, -1, -1, 6, -1, 4, -1, 15, -1, -1, -1, -1, -1, 6, -1, 7, -1,
		-1, -1, 7, 7, -2, -2, -2, -2, -1, 9, -2, -2, -2, -2, -2, -2, -1, -1, 16, 4, -2, -2, -2, -2,
		-1, 7, -2, -2, -2, -2, -2, -2, 5, 7, -2, -2, -2, -2, -2, -2, 3, 6, -2, -2, -2, -2, -2, -2,
		-1, 3, -2, -2, -2, -2, 16, 4, -2, -2, -2, -2, 8, 16, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-1, 10, -2, -2, -2, -2, -2, -2, -2, -2, 15, 10, -2, -2, -2, -2, -2, -2, -2, -2, 15, -1, -1, -1,
		-1, -1, -1, 10, -2, -2, -2, -2, 3, 6, -2, -2, -2, -2, -2, -2, 4, 5, -2, -2, -2, -2, 7, -1,
		-1, -1, 6, -1, 7, 10, -2, -2, -2, -2, -2, -2, -2, -2, 7, 12, -2, -2, -2, -2, -2, -2, -2, -2,
		-1, 16, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 3, 4, -2, -2, -2, -2, 4, 4, -2, -2, -2, -2,
		-1, 7, -2, -2, -2, -2, -2, -2, -1, 8, -2, -2, -2, -2, -2, -2, -1, 6, -2, -2, -2, -2, -2, -2,
		-1, 3, -2, -2, -2, -2, -1, -1, -1, 7, -2, -2, -2, -2, -2, -2, -1, 3, -2, -2, -2, -2, -1, -1 
	};

	prob.Init(10, 12, (int*) problem_array);

	KakuroField field;
	field.Init(prob);
	field.CheckAll();

	field.Debug();
	fprintf(stderr, "status: %d\n", field.GetStatus());
}

}
