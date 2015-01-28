#include "Test.h"
#include "../slitherlink/SlitherlinkProblem.h"

namespace Penciloid
{
const int PenciloidTest::NUMBER_OF_SLITHERLINK_PROBLEM = 7;

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

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 3
	*/
	static const char *problem_3[] = {
		"x3x3x31xx2",
		"30xxx1xx3x",
		"xx33xxx3xx",
		"0x1x2x1xx2",
		"xxx3x2xx23",
		"33xx2x1xxx",
		"2xx3x0x3x3",
		"xx2xxx13xx",
		"x2xx3xxx32",
		"2xx13x3x2x"
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 4
	*/
	static const char *problem_4[] = {
		"x3x3xx0xxxx3xx3x1x",
		"1xxx2x3x31x0x3xxx1",
		"1xxx1x2xxxx1x2xxx0",
		"x3x3xxx1xx3xxx2x2x",
		"3x2x3xx2xx3xx3x3x1",
		"2x3x3xx0xx3xx0x2x2",
		"x3x1xxx2xx2xxx3x1x",
		"2xxx0x1xxxx3x2xxx3",
		"1xxx3x0x32x3x1xxx3",
		"x3x1xx2xxxx2xx1x2x",
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 5
	*/
	static const char *problem_5[] = {
		"-01-1-0-33-2-2-12-",
		"1---2--3--2--0---2",
		"---1-2------2-3---",
		"-2-2--3-03-3--3-1-",
		"2--0-1-1--2-2-1--1",
		"2--3-2-1--1-1-2--1",
		"-3-2--2-11-3--0-1-",
		"---1-2------1-2---",
		"3---1--1--3--2---3",
		"-02-1-3-10-3-0-10-",
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 6
	*/
	static const char *problem_6[] = {
		"3-1--3--22--3--2-1",
		"3-1102------3133-2",
		"-------3--2-------",
		"-03-11-1322-21-31-",
		"-3---3------3---1-",
		"-1---1------0---3-",
		"-32-32-2323-33-23-",
		"-------1--3-------",
		"3-1230------0212-0",
		"3-2--2--31--2--1-1",
	};

	/*
	Source: http://www.nikoli.com/ja/puzzles/slitherlink/ Sample Problem 7
	*/
	static const char *problem_7[] = {
		"1-0-0--2-2---3---0--1112",
		"-------2-2-3---1----1111",
		"-23133-1-0---3---1------",
		"-------3-2-2---0--2-3--1",
		"-11131-3-2--------0-1---",
		"------0-----11312-3-0--3",
		"0--3-1-20223------1-2---",
		"---3-1------11112-2-3--0",
		"3--1-2-01111-----0------",
		"---2-3--------2-1-11122-",
		"0--2-2--0---0-1-2-------",
		"------3---0---0-3-22333-",
		"2222----0---0-1-1-------",
		"2122--3---0---0-1--2-1-1",
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
	case 4:
		problem.Init(10, 18, (char**)problem_4);
		break;
	case 5:
		problem.Init(10, 18, (char**)problem_5);
		break;
	case 6:
		problem.Init(10, 18, (char**)problem_6);
		break;
	case 7:
		problem.Init(14, 24, (char**)problem_7);
		break;
	}
}

}