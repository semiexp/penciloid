#include <cassert>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkDatabaseMethod.hpp"
#include "../slitherlink/SlitherlinkMethod.hpp"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkClueTest(int height, int width, std::vector<const char*> board, int expected_status, bool use_diagonal_chain, int test_id = -1)
{
	SlitherlinkField field;
	SlitherlinkMethod method;
	method.diagonal_chain = use_diagonal_chain;
	field.SetMethod(method);
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
		}, SolverStatus::SUCCESS, false, t * 1000 + 0);

		SlitherlinkClueTest(3, 3, {
			"+-+-+-+",
			"| x x |",
			"+x+-+x+",
			"|3|3| |",
			"+-+x+x+",
			"x x |3|",
			"+x+x+-+",
		}, SolverStatus::SUCCESS, false, t * 1000 + 1);

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
		}, SolverStatus::SUCCESS, false, t * 1000 + 2);

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
		}, SolverStatus::INCONSISTENT, false, t * 1000 + 3);

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
		}, SolverStatus::NORMAL, true, t * 1000 + 100);

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
		}, SolverStatus::NORMAL, true, t * 1000 + 101);

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
	}, SolverStatus::NORMAL, false, 2000);

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
	}, SolverStatus::NORMAL, false, 2001);

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
	}, SolverStatus::NORMAL, false, 2002);

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
	}, SolverStatus::NORMAL, false, 2003);

	SlitherlinkClueTest(4, 4, {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x | x    ",
		"+ + + + +",
		"   1     ",
		"+ + + + +",
		"         ",
		"+ + + + +",
	}, SolverStatus::NORMAL, false, 2004);

	SlitherlinkClueTest(4, 4, {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x1| x    ",
		"+x+ + + +",
		"x  1x    ",
		"+ +x+ + +",
		"         ",
		"+ + + + +",
	}, SolverStatus::NORMAL, false, 2005);

	SlitherlinkClueTest(4, 4, {
		"+ + + +-+",
		"        |",
		"+ + + + +",
		"x | x  2 ",
		"+x+-+x+ +",
		"x0x3|2|  ",
		"+x+-+x+ +",
		"x | x    ",
		"+x+-+-+ +",
	}, SolverStatus::NORMAL, false, 2006);

	SlitherlinkClueTest(4, 4, {
		"+ + + +-+",
		"    x1  |",
		"+ +-+x+ +",
		"   3|    ",
		"+ + + + +",
		"|3       ",
		"+-+ + + +",
		"x        ",
		"+x+ + + +",
	}, SolverStatus::NORMAL, false, 2007);

	SlitherlinkClueTest(3, 5, {
		"+ + + +-+ +",
		"    |3     ",
		"+ +x+-+ + +",
		"  x0x      ",
		"+ +x+ + + +",
		"           ",
		"+ + + + + +",
	}, SolverStatus::NORMAL, false, 2008);

	SlitherlinkClueTest(4, 6, {
		"+ + + + + + +",
		"             ",
		"+ + + +x+ + +",
		"    x x0x    ",
		"+ + +-+x+-+ +",
		"x x1  |3|    ",
		"+x+x+x+-+x+ +",
		"x0x   x x    ",
		"+x+x+ + + + +",
	}, SolverStatus::NORMAL, false, 2009);

	SlitherlinkClueTest(4, 6, {
		"+ + + + + + +",
		"             ",
		"+ + + +x+ + +",
		"    x x0x    ",
		"+ + +-+x+-+ +",
		"x |3  |3|    ",
		"+x+-+x+-+x+ +",
		"x0x   x x    ",
		"+x+x+ + + + +",
	}, SolverStatus::NORMAL, false, 2010);

	SlitherlinkDatabase::ReleaseDatabase();
}

void PenciloidTest::SlitherlinkReducedDatabaseCompletenessTest()
{
	if (SlitherlinkDatabase::IsCreated()) SlitherlinkDatabase::ReleaseDatabase();

	SlitherlinkDatabase::CreateDatabase();

	int *database_complete = new int[531441 * 4];
	memcpy(database_complete, SlitherlinkDatabase::database, 531441 * 4 * sizeof(int));

	SlitherlinkDatabase::ReleaseDatabase();
	SlitherlinkDatabase::CreateReducedDatabase(SlitherlinkDatabaseMethod());

	for (int clue = 0; clue <= 3; ++clue) {
		for (int idx = 0; idx < 531441; ++idx) {
			bool non_sound = false, non_complete = false;

			int loc = clue * 531441 + idx;
			if (database_complete[loc] == SlitherlinkDatabase::database[loc]) continue;
			if (database_complete[loc] != -1 && SlitherlinkDatabase::database[loc] == -1) non_sound = true;
			if (database_complete[loc] == -1 && SlitherlinkDatabase::database[loc] != -1) non_complete = true;

			for (int i = 0; i < 12; ++i) {
				int val_complete = (database_complete[loc] >> (2 * i)) & 3;
				int val_reduced = (SlitherlinkDatabase::database[loc] >> (2 * i)) & 3;

				if (val_complete == 0 && val_reduced != 0) non_sound = true;
				if (val_complete != 0 && val_reduced == 0) non_complete = true;
			}

			if (non_sound || non_complete) {
				if (non_sound) std::cerr << "Soundness";
				else std::cerr << "Completeness";

				std::cerr << " is not satisfied" << std::endl << "clue: " << clue << ", segments : [";
				for (int i = 0, v = idx; i < 12; ++i, v /= 3) {
					std::cerr << (v % 3);
					if (i != 11) std::cerr << ", ";
				}
				std::cerr << "]" << std::endl;
				goto end;
			}
		}
	}

end:
	delete[] database_complete;
	SlitherlinkDatabase::ReleaseDatabase();
}

}