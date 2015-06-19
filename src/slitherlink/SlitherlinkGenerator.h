#pragma once

namespace Penciloid
{
class SlitherlinkProblem;
class SlitherlinkProblemConstraint;

class SlitherlinkGenerator
{
public:
	static bool GenerateNaive(int height, int width, SlitherlinkProblem &ret, bool symmetry = false);
	static bool GenerateOfShape(SlitherlinkProblemConstraint &constraint, SlitherlinkProblem &ret, XorShift &rnd, bool use_assumption = false);
	static void SimplifyProblem(SlitherlinkProblem &problem, bool symmetry = false);
};

}
