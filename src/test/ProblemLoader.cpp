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

namespace Penciloid
{
void PenciloidTest::InputSlitherlink(std::ifstream &ifs, SlitherlinkProblem &problem)
{
	int height, width;
	ifs >> height >> width;

	problem.Init(height, width);

	for (int i = 0; i < height; ++i) {
		std::string line;
		ifs >> line;

		for (int j = 0; j < width; ++j) {
			if (line[j] >= '0' && line[j] <= '3') problem.SetHint(i, j, line[j] - '0');
		}
	}
}

void PenciloidTest::InputMasyu(std::ifstream &ifs, MasyuProblem &problem)
{
	int height, width;
	ifs >> height >> width;

	problem.Init(height, width);

	for (int i = 0; i < height; ++i) {
		std::string line;
		ifs >> line;

		for (int j = 0; j < width; ++j) {
			if (line[j] == 'b' || line[j] == '1') problem.SetHint(i, j, MasyuProblem::HINT_BLACK);
			if (line[j] == 'w' || line[j] == '2') problem.SetHint(i, j, MasyuProblem::HINT_WHITE);
		}
	}
}

void PenciloidTest::InputKakuro(std::ifstream &ifs, KakuroProblem &problem)
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

	problem.Init(height, width, problem_data);
	delete[] solution;
	delete[] problem_data;
}

}
