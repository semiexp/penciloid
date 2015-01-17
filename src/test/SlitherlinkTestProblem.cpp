#include "Test.h"
#include "../slitherlink/SlitherlinkProblem.h"

namespace Penciloid
{
const int PenciloidTest::NUMBER_OF_SLITHERLINK_PROBLEM = 2;

void PenciloidTest::SlitherlinkLoadProblem(SlitherlinkProblem &problem, int problem_id)
{
	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 1
	*/
	static const char *problem_1[] = {
		"xxxx02xxxx",
		"230xxxx223",
		"xxx3xx3xxx",
		"3xxx22xxx1",
		"x2x2xx0x2x",
		"x2x3xx3x3x",
		"3xxx10xxx2",
		"xxx2xx3xxx",
		"303xxxx331",
		"xxxx02xxxx"
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 2
	*/
	static const char *problem_2[] = {
		"1x220xxxxx",
		"0xxxxx0x3x",
		"1xxxxx1xx3",
		"1x333x0xxx",
		"0xxx1xx1xx",
		"xx2xx1xxx3",
		"xxx2x201x2",
		"2xx3xxxxx3",
		"x3x0xxxxx3",
		"xxxxx010x1"
	};


	switch (problem_id)
	{
	case 1:
		problem.Init(10, 10, (char**)problem_1);
		break;
	case 2:
		problem.Init(10, 10, (char**)problem_2);
		break;
	}
}

}