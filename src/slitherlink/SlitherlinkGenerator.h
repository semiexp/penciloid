#pragma once

namespace Penciloid
{
class SlitherlinkProblem;

class SlitherlinkGenerator
{
public:
	static bool GenerateNaive(int height, int width, SlitherlinkProblem &ret, bool symmetry = false);
	static void SimplifyProblem(SlitherlinkProblem &problem, bool symmetry = false);
};

}
