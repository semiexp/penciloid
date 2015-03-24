#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Test.h"
#include "../kakuro/KakuroField.h"
#include "../kakuro/KakuroProblem.h"

/*
Format of input file:

problem id1
height1 width1
solution1 [height1 * width1]
problem id2
height2 width2
solution2 [height2 * width2]
...
*/

namespace Penciloid
{
void PenciloidTest::KakuroConsecutiveSolve(const char* filename)
{
	std::vector<int> problem_height, problem_width;
	std::vector<std::vector<int> > problem_body;
	std::vector<std::string> problem_ids;

	{
		std::ifstream ifs(filename);

		while (!ifs.eof()) {
			std::string ins, probid;
			ifs >> ins >> probid;

			if (ins != std::string("problem")) break;

			problem_ids.push_back(probid);
			int height, width, cell;
			ifs >> height >> width;

			problem_height.push_back(height);
			problem_width.push_back(width);

			std::vector<int> body;
			for (int i = 0; i < height * width; ++i) {
				ifs >> cell;

				body.push_back(cell);
			}

			problem_body.push_back(body);
		}
	}

	for (int i = 0; i < problem_body.size(); ++i) {
		int height = problem_height[i], width = problem_width[i];
		int* problem_data = new int[2 * height * width];

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int cell_vertical = (y * width + x) * 2;
				int cell_horizontal = (y * width + x) * 2 + 1;

				if (problem_body[i][y * width + x] != 0) {
					// number cell
					problem_data[cell_vertical] = problem_data[cell_horizontal] = KakuroProblem::NUMBER_CELL;
				} else {
					int vertical = 0, horizontal = 0;

					for (int y2 = y + 1; y2 < height; ++y2) {
						if (problem_body[i][y2 * width + x] == 0) break;
						vertical += problem_body[i][y2 * width + x];
					}
					for (int x2 = x + 1; x2 < width; ++x2) {
						if (problem_body[i][y * width + x2] == 0) break;
						horizontal += problem_body[i][y * width + x2];
					}
					problem_data[cell_vertical] = (vertical == 0) ? KakuroProblem::HINT_NONE : vertical;
					problem_data[cell_horizontal] = (horizontal == 0) ? KakuroProblem::HINT_NONE : horizontal;
				}
			}
		}

		KakuroProblem prob;
		prob.Init(height, width, problem_data);
		delete[] problem_data;

		KakuroField field;
		field.Init(prob);
		field.CheckAll();

		std::cout << "Problem " << problem_ids[i] << ": ";
		std::cerr << "Problem " << problem_ids[i] << ": ";

		if (field.GetStatus() == SolverStatus::SUCCESS) {
			std::cout << "OK" << std::endl;
			std::cerr << "OK" << std::endl;
		} else {
			std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
			std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
			field.Debug();
		}

		std::cout.flush();
		std::cerr.flush();
	}
}

}
