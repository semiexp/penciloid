#include "Test.h"
#include "../masyu/MasyuProblem.h"

namespace Penciloid
{
const int PenciloidTest::NUMBER_OF_MASYU_PROBLEM = 1;

void PenciloidTest::MasyuLoadProblem(MasyuProblem &problem, int problem_id)
{
	/*
	Source: http://www.nikoli.com/ja/puzzles/masyu/ Sample Problem 1
	*/
	static const char *problem_1[] = {
		"1-2--22---",
		"---2----22",
		"1-12--2---",
		"---1-12---",
		"222--1-21-",
		"------1-1-",
		"-1--2----2",
		"-----12---",
		"-222-2122-",
		"------2---",
	};

	switch (problem_id)
	{
	case 1:
		problem.Init(10, 10, (char**)problem_1);
		break;
	}
}

}