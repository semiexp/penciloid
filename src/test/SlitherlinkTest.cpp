#include <cassert>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkDatabaseMethod.hpp"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkClueTest(int height, int width, std::vector<const char*> board, int expected_status, int test_id = -1)
{
	SlitherlinkField field;
	field.Init(height, width);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			char clue = board[y * 2 + 1][x * 2 + 1];
			if ('0' <= clue && clue <= '3') field.SetClue(y, x, clue - '0');
		}
	}

	field.CheckAll();

	bool is_status_ok = true;
	if (expected_status == SolverStatus::NORMAL && field.GetStatus() != expected_status) is_status_ok = false;
	if (expected_status == SolverStatus::SUCCESS && field.GetStatus() != expected_status) is_status_ok = false;
	if (expected_status == SolverStatus::INCONSISTENT && (field.GetStatus() & SolverStatus::INCONSISTENT) == 0) is_status_ok = false;

	if (!is_status_ok) {
		std::cerr << "SlitherlinkClueTest failed in test #" << test_id << std::endl;
		std::cerr << "expected: " << expected_status << ", actual: " << field.GetStatus() << " (field status)" << std::endl;
	}

	if (expected_status == SolverStatus::INCONSISTENT) return;

	for (int y = 0; y <= height * 2; ++y) {
		for (int x = 0; x <= width * 2; ++x) {
			if (y % 2 == x % 2) continue; // this is not a segment

			int expected =
				board[y][x] == ' ' ? SlitherlinkField::LOOP_UNDECIDED :
				board[y][x] == 'x' ? SlitherlinkField::LOOP_BLANK : SlitherlinkField::LOOP_LINE;

			if (field.GetSegmentStyle(y, x) != expected) {
				std::cerr << "SlitherlinkClueTest failed in test #" << test_id << std::endl;
				std::cerr << "expected: " << expected << ", actual: " << field.GetSegmentStyle(y, x) << " at (" << y << "," << x << ")" << std::endl;
				exit(1);
			}
		}
	}
}

bool PenciloidTest::SlitherlinkCheckGrid(SlitherlinkField &field, const int *expected)
{
	int height = field.GetHeight() * 2 + 1, width = field.GetWidth() * 2 + 1;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (i % 2 != j % 2) {
				int expected_style = (expected[i * width + j] == 1) ? SlitherlinkField::LOOP_LINE : SlitherlinkField::LOOP_BLANK;
				if (field.GetSegmentStyle(i, j) != expected_style) return false;
			}
		}
	}

	return true;
}

void PenciloidTest::SlitherlinkFieldTestByProblems()
{
	for (int t = 0; t < 2; ++t) {
		// t: enable database or not
		// t is specified in test_id by adding t * 1000

		if (t == 0) {
			if (SlitherlinkDatabase::IsCreated()) SlitherlinkDatabase::ReleaseDatabase();
		} else if (t == 1) {
			if (!SlitherlinkDatabase::IsCreated()) SlitherlinkDatabase::CreateDatabase();
		}

		SlitherlinkClueTest(3, 3, {
			"+x+x+-+",
			"x0x2| |",
			"+x+-+x+",
			"x | x |",
			"+-+x+x+",
			"|3x1x |",
			"+-+-+-+",
		}, SolverStatus::SUCCESS, t * 1000 + 0);

		SlitherlinkClueTest(3, 3, {
			"+-+-+-+",
			"| x x |",
			"+x+-+x+",
			"|3|3| |",
			"+-+x+x+",
			"x x |3|",
			"+x+x+-+",
		}, SolverStatus::SUCCESS, t * 1000 + 1);

		// 2: closed loop test
		SlitherlinkClueTest(5, 3, {
			"+-+x+-+",
			"|3|3|3|",
			"+x+-+x+",
			"|2x2x2|",
			"+-+-+-+",
			"x x1x x",
			"+x+x+x+",
			"x x x x",
			"+x+x+x+",
			"x x x x",
			"+x+x+x+",
		}, SolverStatus::SUCCESS, t * 1000 + 2);

		// 3: closed loop (inconsistent) test
		SlitherlinkClueTest(5, 3, {
			"+ + + +",
			" 3 3 3 ",
			"+ + + +",
			" 2 2 2 ",
			"+ + + +",
			"       ",
			"+ + + +",
			"     2 ",
			"+ + + +",
			"       ",
			"+ + + +",
		}, SolverStatus::INCONSISTENT, t * 1000 + 3);

		// 1xx: diagonal chain test
		SlitherlinkClueTest(4, 4, {
			"+x+x+ + +",
			"x0x      ",
			"+x+ + + +",
			"x  2     ",
			"+ + +x+ +",
			"    x1   ",
			"+ + + + +",
			"         ",
			"+ + + + +",
		}, SolverStatus::NORMAL, t * 1000 + 100);

		SlitherlinkClueTest(4, 4, {
			"+x+x+ + +",
			"x0x      ",
			"+x+-+ + +",
			"x |2x    ",
			"+ +x+-+ +",
			"    |3   ",
			"+ + + + +",
			"         ",
			"+ + + + +",
		}, SolverStatus::NORMAL, t * 1000 + 101);

		if (t == 1) SlitherlinkDatabase::ReleaseDatabase();
	}
}

void PenciloidTest::SlitherlinkReducedDatabaseTest()
{
	if (SlitherlinkDatabase::IsCreated()) SlitherlinkDatabase::ReleaseDatabase();
	SlitherlinkDatabase::CreateReducedDatabase(SlitherlinkDatabaseMethod());

	SlitherlinkClueTest(4, 4, {
		"+x+ + + +",
		"x1       ",
		"+ + + + +",
		"         ",
		"+ + + + +",
		"         ",
		"+ + + + +",
		"         ",
		"+ + + + +",
	}, SolverStatus::NORMAL, 2000);

	SlitherlinkClueTest(4, 4, {
		"+ +-+ + +",
		" 2       ",
		"+ + + + +",
		"|        ",
		"+ + + + +",
		"         ",
		"+ + + + +",
		"         ",
		"+ + + + +",
	}, SolverStatus::NORMAL, 2001);

	SlitherlinkClueTest(4, 4, {
		"+-+ + + +",
		"|3       ",
		"+ + + + +",
		"         ",
		"+ + + + +",
		"         ",
		"+ + + + +",
		"         ",
		"+ + + + +",
	}, SolverStatus::NORMAL, 2002);

	SlitherlinkClueTest(4, 4, {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x | x    ",
		"+x+ + + +",
		"x  3|    ",
		"+ +-+x+ +",
		"    x    ",
		"+ + + + +",
	}, SolverStatus::NORMAL, 2003);

	SlitherlinkDatabase::ReleaseDatabase();
}
}