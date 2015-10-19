#include "../slitherlink/SlitherlinkEvaluator.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

#include <vector>

static double cov(std::vector<double> &xs, std::vector<double> &ys)
{
	double x_ave = 0, y_ave = 0;
	for (double x : xs) x_ave += x; x_ave /= xs.size();
	for (double x : xs) y_ave += x; y_ave /= ys.size();

	double ret = 0;
	for (int i = 0; i < xs.size(); ++i) ret += (xs[i] - x_ave) * (ys[i] - y_ave);

	return ret / xs.size();
}

static double calc_correl(std::vector<double> xs, std::vector<double> ys)
{
	return cov(xs, ys) / sqrt(cov(xs, xs) * cov(ys, ys));
}

namespace Penciloid
{
void PenciloidTest::SlitherlinkEvaluatorTestWithProblem()
{
	std::vector<int> heights, widths; std::vector<double> scores;

	for (;;) {
		SlitherlinkProblem prob;
		if (!PenciloidTest::InputSlitherlink(std::cin, prob)) break;

		SlitherlinkEvaluator e;
		e.Init(prob);

		heights.push_back(prob.GetHeight());
		widths.push_back(prob.GetWidth());
		scores.push_back(e.Evaluate());
	}

	for (int i = 0; i < heights.size(); ++i) {
		std::cout << (i + 1) << " " << scores[i] << std::endl;
	}

	int top = 0;
	for (int i = 0; i < heights.size(); ++i) {
		if (i == heights.size() - 1 || (heights[i] != heights[i + 1] || widths[i] != widths[i + 1])) {
			// calc
			std::vector<double> xs, ys;
			for (int j = top; j <= i; ++j) {
				if (scores[j] > 0) {
					xs.push_back(j);
					ys.push_back(scores[j]);
				}
			}

			double cor_rel = calc_correl(xs, ys);
			std::cerr << heights[i] << "x" << widths[i] << " (" << xs.size() << " / " << (i - top + 1) << "): " << cor_rel << std::endl;
			top = i + 1;
		}
	}
}

}
