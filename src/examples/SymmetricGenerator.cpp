#include "../slitherlink/SlitherlinkGenerator.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "../slitherlink/SlitherlinkProblemConstraint.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkDatabase.h"
#include "../common/XorShift.h"
#include "Examples.h"

#include <algorithm>
#include <ctime>

static double ComputeSymmetricityOfField(Penciloid::SlitherlinkField &field, bool check_rot_90 = true)
{
	int symmetry_score = 0, possible_max = 0;
	int height = field.GetHeight(), width = field.GetWidth();
	int height2 = height * 2, width2 = width * 2;

	for (int y = 0; y <= height2; ++y) {
		for (int x = 0; x <= width2; ++x) {
			if (y % 2 == x % 2) continue;

			int adj_y1 = (y - 1) / 2, adj_x1 = (x - 1) / 2; bool adj1_valid = (y > 0 && x > 0) && field.GetClue(adj_y1, adj_x1) != Penciloid::SlitherlinkField::CLUE_NONE;
			int adj_y2 = y / 2, adj_x2 = x / 2; bool adj2_valid = (y < height2 && x < width2) && field.GetClue(adj_y2, adj_x2) != Penciloid::SlitherlinkField::CLUE_NONE;

			if (height2 == width2 && check_rot_90) {
				possible_max += 2;

				if (field.GetSegmentStyle(y, x) == field.GetSegmentStyle(x, width2 - y)
					&& ((adj1_valid && field.GetClue(adj_y1, adj_x1) == field.GetClue(adj_x1, width - 1 - adj_y1))
					|| (adj2_valid && field.GetClue(adj_y2, adj_x2) == field.GetClue(adj_x2, width - 1 - adj_y2)))
					) ++symmetry_score;

				if (field.GetSegmentStyle(y, x) == field.GetSegmentStyle(height2 - x, y)
					&& ((adj1_valid && field.GetClue(adj_y1, adj_x1) == field.GetClue(height - 1 - adj_x1, adj_y1))
					|| (adj2_valid && field.GetClue(adj_y2, adj_x2) == field.GetClue(height - 1 - adj_x2, adj_y2)))
					) ++symmetry_score;
			}

			if (field.GetSegmentStyle(y, x) == field.GetSegmentStyle(height2 - y, width2 - x)
				&& ((adj1_valid && field.GetClue(adj_y1, adj_x1) == field.GetClue(height - 1 - adj_y1, width - 1 - adj_x1))
				|| (adj2_valid && field.GetClue(adj_y2, adj_x2) == field.GetClue(height - 1 - adj_y2, width - 1 - adj_x2)))
				) ++symmetry_score;
			++possible_max;
		}
	}
	return symmetry_score / (double)possible_max;
}

namespace Penciloid
{
void PenciloidExamples::SymmetricGenerator()
{
	const int height = 10, width = 10;
	const int clues_min = 28, clues_max = 36;
	const bool rotation_90 = true;
	const double threshold = 0.07;

	using namespace Penciloid;
	XorShift xs((int)time(NULL));
	time_t start = clock();
	int generated_problems = 0;

	SlitherlinkDatabase::CreateDatabase();

	for (;;) {
		SlitherlinkProblemConstraint cond;
		SlitherlinkGenerator::GenerateConstraint(
			height,
			width,
			clues_min + (int)(xs.NextInt() % (clues_max - clues_min + 1)),
			rotation_90 ? SlitherlinkGenerator::SHAPE_ROTATION_90 : SlitherlinkGenerator::SHAPE_ROTATION_180,
			xs,
			cond);

		SlitherlinkProblem prob;
		if (!SlitherlinkGenerator::GenerateOfShape(cond, prob, xs, false)) continue;

		SlitherlinkField field;
		field.Init(prob);
		field.CheckAll();

		double sym = ComputeSymmetricityOfField(field, rotation_90);
		if (sym > threshold) continue;

		field.Debug();

		double elapsed_time_sec = (double)(clock() - start) / CLOCKS_PER_SEC;
		++generated_problems;
			
		printf("%d problems generated in %.2f sec (%.2f [prob/s])\n", generated_problems, elapsed_time_sec, generated_problems / elapsed_time_sec);
	}
}
}
