#include "Test.h"
#include "../masyu/MasyuProblem.h"

namespace Penciloid
{
const int PenciloidTest::NUMBER_OF_MASYU_PROBLEM = 3;

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

	/*
	Source: http://www.nikoli.com/ja/puzzles/masyu/ Sample Problem 2
	*/
	static const char *problem_2[] = {
		"----2-1-2-",
		"-12-------",
		"------1---",
		"2------1-2",
		"--2---2--2",
		"----1-2---",
		"----1-2---",
		"-1------12",
		"-----2----",
		"-2--1-2---"
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/masyu/ Sample Problem 3
	*/
	static const char *problem_3[] = {
		"--2-2--22-",
		"----------",
		"-22---11--",
		"----------",
		"---2-1----",
		"-1-----2-1",
		"-1---2----",
		"------2---",
		"-2-1--222-",
		"----------" 
	};

	switch (problem_id)
	{
	case 1:
		problem.Init(10, 10, (char**)problem_1);
		break;
	case 2:
		problem.Init(10, 10, (char**)problem_2);
		break;
	case 3:
		problem.Init(10, 10, (char**)problem_3);
		break;
	}
}

}