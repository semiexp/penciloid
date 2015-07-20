#pragma once

namespace Penciloid
{
class SlitherlinkProblem;
class SlitherlinkProblemConstraint;
class XorShift;

class SlitherlinkGenerator
{
public:
	static bool GenerateNaive(int height, int width, SlitherlinkProblem &ret, bool symmetry = false);
	static bool GenerateOfShape(SlitherlinkProblemConstraint &constraint, SlitherlinkProblem &ret, XorShift &rnd, bool use_assumption = false);
	static void SimplifyProblem(SlitherlinkProblem &problem, bool symmetry = false);
	static void GenerateConstraint(int height, int width, int n_hints, int symmetricity, XorShift &rnd, SlitherlinkProblemConstraint &ret);

	static const int SHAPE_ROTATION_180 = 1;
	static const int SHAPE_ROTATION_90 = SHAPE_ROTATION_180 | 2;
	static const int SHAPE_MIRROR_HORIZONTAL = 4;
	static const int SHAPE_MIRROR_VERTICAL = 8;
};

}
