#include <ctime>
#include <iostream>
#include "Test.h"
#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkGenerator.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkGeneratorTest(int height, int width)
{
	srand((int)time(NULL));
	SlitherlinkDatabase::CreateDatabase();

	SlitherlinkProblem prob;
	SlitherlinkGenerator::GenerateNaive(height, width, prob, true);
	SlitherlinkGenerator::SimplifyProblem(prob, true);

	int num_hints = 0;
	std::cout << height << " " << width << std::endl;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (prob.GetHint(i, j) == SlitherlinkField::HINT_NONE) std::cout << '.';
			else {
				std::cout << (char)(prob.GetHint(i, j) + '0');
				++num_hints;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "hint: " << num_hints << std::endl;
	std::cout << std::endl;

	SlitherlinkField field;
	field.Init(prob);
	field.Assume();

	field.Debug();
}

}
