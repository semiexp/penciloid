#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Test.h"
#include "../masyu/MasyuField.h"
#include "../masyu/MasyuProblem.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "../kakuro/KakuroField.h"
#include "../kakuro/KakuroProblem.h"

static void SolveSlitherlinkTest(std::ifstream &ifs, std::string &probid)
{
	using namespace Penciloid;

	int height, width;
	ifs >> height >> width;

	SlitherlinkProblem prob;
	prob.Init(height, width);

	for (int i = 0; i < height; ++i) {
		std::string line;
		ifs >> line;

		for (int j = 0; j < width; ++j) {
			if (line[j] >= '0' && line[j] <= '3') prob.SetHint(i, j, line[j] - '0');
		}
	}

	SlitherlinkField field;
	field.Init(prob);

	field.Assume();

	std::cout << "Slitherlink [" << probid << "]: ";
	std::cerr << "Slitherlink [" << probid << "]: ";

	if (field.GetStatus() == SolverStatus::SUCCESS) {
		std::cout << "OK" << std::endl;
		std::cerr << "OK" << std::endl;
	} else {
		std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
		std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
		field.Debug();
	}
}

static void SolveMasyuTest(std::ifstream &ifs, std::string &probid)
{
	using namespace Penciloid;

	int height, width;
	ifs >> height >> width;

	MasyuProblem prob;
	prob.Init(height, width);

	for (int i = 0; i < height; ++i) {
		std::string line;
		ifs >> line;

		for (int j = 0; j < width; ++j) {
			if (line[j] == 'b' || line[j] == '1') prob.SetHint(i, j, MasyuProblem::HINT_BLACK);
			if (line[j] == 'w' || line[j] == '2') prob.SetHint(i, j, MasyuProblem::HINT_WHITE);
		}
	}

	MasyuField field;
	field.Init(prob);

	field.Assume();

	std::cout << "Masyu [" << probid << "]: ";
	std::cerr << "Masyu [" << probid << "]: ";

	if (field.GetStatus() == SolverStatus::SUCCESS) {
		std::cout << "OK" << std::endl;
		std::cerr << "OK" << std::endl;
	} else {
		std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
		std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
		field.Debug();
	}
}

static void SolveKakuroTest(std::ifstream &ifs, std::string &probid)
{
	using namespace Penciloid;

	int height, width, *solution = nullptr, *problem_data = nullptr;

	ifs >> height >> width;
	solution = new int[height * width];
	for (int i = 0; i < height * width; ++i) ifs >> solution[i];

	problem_data = new int[2 * height * width];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int cell_vertical = (y * width + x) * 2;
			int cell_horizontal = (y * width + x) * 2 + 1;

			if (solution[y * width + x] != 0) {
				// number cell
				problem_data[cell_vertical] = problem_data[cell_horizontal] = KakuroProblem::NUMBER_CELL;
			} else {
				int vertical = 0, horizontal = 0;

				for (int y2 = y + 1; y2 < height; ++y2) {
					if (solution[y2 * width + x] == 0) break;
					vertical += solution[y2 * width + x];
				}
				for (int x2 = x + 1; x2 < width; ++x2) {
					if (solution[y * width + x2] == 0) break;
					horizontal += solution[y * width + x2];
				}
				problem_data[cell_vertical] = (vertical == 0) ? KakuroProblem::HINT_NONE : vertical;
				problem_data[cell_horizontal] = (horizontal == 0) ? KakuroProblem::HINT_NONE : horizontal;
			}
		}
	}

	KakuroProblem prob;
	prob.Init(height, width, problem_data);
	delete[] solution;
	delete[] problem_data;

	KakuroField field;
	field.Init(prob);
	field.CheckAll();

	std::cout << "Kakuro [" << probid << "]: ";
	std::cerr << "Kakuro [" << probid << "]: ";
	
	if (field.GetStatus() == SolverStatus::SUCCESS) {
		std::cout << "OK" << std::endl;
		std::cerr << "OK" << std::endl;
	} else {
		std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
		std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
		field.Debug();
	}
}

namespace Penciloid
{
void PenciloidTest::ConsecutiveSolve(const char* filename)
{
	std::ifstream ifs(filename);

	while (!ifs.eof()) {
		std::string puzzle_type, probid;
		ifs >> puzzle_type >> probid;

		if (puzzle_type == std::string("slitherlink")) {
			SolveSlitherlinkTest(ifs, probid);
		} else if (puzzle_type == std::string("masyu")) {
			SolveMasyuTest(ifs, probid);
		} else if (puzzle_type == std::string("kakuro")) {
			SolveKakuroTest(ifs, probid);
		} else {
			break;
		}
	}
}

}
