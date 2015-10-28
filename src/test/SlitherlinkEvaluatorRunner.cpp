#include "../slitherlink/SlitherlinkEvaluator.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "Test.h"

#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx;

void EvaluatorWorker(std::vector<Penciloid::SlitherlinkProblem> &problems, std::vector<double> &scores, int *next_problem)
{
	for (;;) {
		int problem_id;
		
		mtx.lock();
		if (*next_problem >= (problems).size()) {
			mtx.unlock();
			break;
		}

		problem_id = (*next_problem)++;
		mtx.unlock();

		Penciloid::SlitherlinkEvaluator e;
		e.Init((problems)[problem_id]);

		double score = e.Evaluate();

		mtx.lock();
		(scores)[problem_id] = score;
		mtx.unlock();
	}
}

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
void PenciloidTest::SlitherlinkEvaluatorTestWithProblem(int n_threads)
{
	std::vector<int> heights, widths;
	std::vector<double> scores;
	std::vector<Penciloid::SlitherlinkProblem> problems;
	int next_problem = 0;

	for (;;) {
		SlitherlinkProblem prob;
		if (!PenciloidTest::InputSlitherlink(std::cin, prob)) break;

		problems.push_back(prob);
		heights.push_back(prob.GetHeight());
		widths.push_back(prob.GetWidth());
		scores.push_back(-3.0);
	}

	if (n_threads == 1) {
		for (int i = 0; i < problems.size(); ++i) {
			SlitherlinkEvaluator e;
			e.Init(problems[i]);
			scores[i] = e.Evaluate();
		}
	} else {
		std::vector<std::thread> ths(n_threads);

		for (int i = 0; i < n_threads; ++i) {
			ths[i] = std::thread(EvaluatorWorker, std::ref(problems), std::ref(scores), &next_problem);
		}

		for (int i = 0; i < n_threads; ++i) ths[i].join();
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
