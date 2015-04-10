#pragma once

namespace Penciloid
{
class SlitherlinkProblem;

class SlitherlinkGenerator
{
public:
	static bool GenerateNaive(int height, int width, SlitherlinkProblem &ret, bool symmetry = false);
	static bool GenerateOfShape(int height, int width, int *shape, SlitherlinkProblem &ret);
	static void SimplifyProblem(SlitherlinkProblem &problem, bool symmetry = false);
};

}
