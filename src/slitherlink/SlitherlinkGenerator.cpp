#include <ctime>
#include <vector>
#include <cmath>

#include "SlitherlinkGenerator.h"
#include "SlitherlinkField.h"
#include "SlitherlinkProblem.h"
#include "SlitherlinkDatabase.h"

namespace Penciloid
{
bool SlitherlinkGenerator::GenerateNaive(int height, int width, SlitherlinkProblem &ret, bool symmetry)
{
	SlitherlinkField field;
	field.Init(height, width);

	std::vector<std::pair<std::pair<int, int>, int> > current_hints;
	
	for (;;) {
		// put a hint randomly
		printf("%d\n", current_hints.size());
		std::vector<std::pair<std::pair<int, int>, int> > cand;
		
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				if (symmetry && current_hints.size() % 2 == 1) {
					// TODO: odd * odd case
					if (i + current_hints[current_hints.size() - 1].first.first != height - 1) continue;
					if (j + current_hints[current_hints.size() - 1].first.second != width - 1) continue;
				}

				if (field.GetHint(i, j) != SlitherlinkField::HINT_NONE) continue;
				if (!symmetry && field.GetSegmentStyle(i * 2, j * 2 + 1) != SlitherlinkField::LOOP_UNDECIDED
					&& field.GetSegmentStyle(i * 2 + 1, j * 2) != SlitherlinkField::LOOP_UNDECIDED
					&& field.GetSegmentStyle(i * 2 + 2, j * 2 + 1) != SlitherlinkField::LOOP_UNDECIDED
					&& field.GetSegmentStyle(i * 2 + 1, j * 2 + 2) != SlitherlinkField::LOOP_UNDECIDED) continue;

				bool zero_validity = current_hints.size() >= 3;
				if (field.GetHintSafe(i - 1, j - 1) == 0
					|| field.GetHintSafe(i - 1, j) == 0
					|| field.GetHintSafe(i - 1, j + 1) == 0
					|| field.GetHintSafe(i, j - 1) == 0
					|| field.GetHintSafe(i, j + 1) == 0
					|| field.GetHintSafe(i + 1, j - 1) == 0
					|| field.GetHintSafe(i + 1, j) == 0
					|| field.GetHintSafe(i + 1, j + 1) == 0) zero_validity = false;

				for (int k = zero_validity ? 0 : 1; k < 4; ++k) {
					SlitherlinkField field2;
					field2.Init(field);

					field2.SetHint(i, j, k);
					if (field2.GetStatus() & SolverStatus::INCONSISTENT) continue;

					// field2.Assume();
					// if (field2.GetStatus() & SolverStatus::INCONSISTENT) continue;

					cand.push_back(std::make_pair(std::make_pair(i, j), k));
				}
			}
		}

		if (cand.size() == 0) {
			goto rollback;
		}

		for (int t = 0;; ++t) {
			if (t == 5 || cand.size() == 0) goto rollback;

			auto next_step = cand[rand() % cand.size()];

			SlitherlinkField field2;
			field2.Init(field);
			field2.SetHint(next_step.first.first, next_step.first.second, next_step.second);
			field2.Assume();
			field2.CheckInOutRule();
			field2.CheckConnectability();

			if (field2.GetStatus() & SolverStatus::INCONSISTENT) continue;

			current_hints.push_back(next_step);
			field.SetHint(next_step.first.first, next_step.first.second, next_step.second);
			field.Assume();
			field.CheckInOutRule();
			field.CheckConnectability();

			if (field.GetStatus() == SolverStatus::SUCCESS) {
				if (symmetry && current_hints.size() % 2 == 1) {
					break;
				}

				ret.Init(height, width);
				for (int y = 0; y < height; ++y) {
					for (int x = 0; x < width; ++x) if (field.GetHint(y, x) != SlitherlinkField::HINT_NONE) {
						ret.SetHint(y, x, field.GetHint(y, x));
					}
				}
				return true;
			}

			break;
		}

		continue;

	rollback:
		for (int t = 0; t < 15 && !current_hints.empty(); ++t) {
			current_hints.pop_back();
		}

		field.Init(height, width);
		for (auto step : current_hints) {
			field.SetHint(step.first.first, step.first.second, step.second);
		}
		field.Assume();
		field.CheckInOutRule();
		field.CheckConnectability();

		field.Debug();
		continue;

	}

	return false;
}

bool SlitherlinkGenerator::GenerateOfShape(int height, int width, int *shape, SlitherlinkProblem &ret, bool use_assumption)
{
	SlitherlinkProblem current_problem;
	current_problem.Init(height, width);

	int best_score = 0;
	int max_step = height * width * 10;
	int number_of_unplaced_hints = 0;
	int no_progress = 0;

	for (int i = 0; i < height * width; ++i) if (shape[i]) ++number_of_unplaced_hints;

	SlitherlinkField field;
	field.Init(height, width);

	for (int step = 0; step < max_step; ++step) {
		int current_progress = 0;
		bool is_progress = false;

		// double temperature = 2.0 * (max_step - step) / (double)max_step;
		double temperature = 5.0 * exp(-2.0 * (double)step / max_step);

		current_progress = field.GetProgress();
		if (best_score < current_progress) best_score = current_progress;

		std::vector<std::pair<int, int> > locs;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) if (shape[i * width + j]) { //current_problem.GetHint(i, j) != SlitherlinkField::HINT_NONE) {
				bool check_flg = false;
				if (field.GetHint(i, j) == SlitherlinkField::HINT_NONE) check_flg = true;

				for (int k = -7; k <= 7; ++k) {
					for (int l = -7; l <= 7; ++l) {
						int y = i * 2 + 1 + k, x = j * 2 + 1 + l;
						if (0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width && y % 2 != x % 2 && field.GetSegmentStyle(y, x) == SlitherlinkField::LOOP_UNDECIDED) {
							check_flg = true;
							break;
						}
					}
				}
				if (check_flg) locs.push_back(std::make_pair(i, j));
			}
		}

		for (int i = 0; i < locs.size(); ++i) {
			int j = i + rand() % (locs.size() - i);
			if (i != j) std::swap(locs[i], locs[j]);
		}

		for (auto &t : locs) {
			int i = t.first, j = t.second;
			int current_hint = current_problem.GetHint(i, j);

			bool zero_validity = true;
			if (field.GetHintSafe(i - 1, j - 1) == 0
				|| field.GetHintSafe(i - 1, j) == 0
				|| field.GetHintSafe(i - 1, j + 1) == 0
				|| field.GetHintSafe(i, j - 1) == 0
				|| field.GetHintSafe(i, j + 1) == 0
				|| field.GetHintSafe(i + 1, j - 1) == 0
				|| field.GetHintSafe(i + 1, j) == 0
				|| field.GetHintSafe(i + 1, j + 1) == 0) zero_validity = false;

			std::vector<int> nums;
			for (int n = zero_validity ? 0 : 1; n <= 3; ++n) if (n != current_hint) nums.push_back(n);
			for (int i = 0; i < nums.size(); ++i) {
				int j = i + rand() % (nums.size() - i);
				if (i != j) std::swap(nums[i], nums[j]);
			}

			int previous_hint = current_problem.GetHint(i, j);
			SlitherlinkField common;

			if (previous_hint == SlitherlinkField::HINT_NONE) {
				common.Init(field);
			} else {
				current_problem.SetHint(i, j, SlitherlinkField::HINT_NONE);
				common.Init(current_problem);
			}

			for (int n : nums) {
				current_problem.SetHint(i, j, n);
				SlitherlinkField field2;
				field2.Init(common);
				field2.SetHint(i, j, n);
				if (use_assumption) field2.Assume();

				bool transition = false;

				if (field2.GetStatus() & SolverStatus::INCONSISTENT) continue;

				int new_progress = field2.GetProgress();

				SlitherlinkField field3;
				field3.Init(field2);
				field3.CheckInOutRule();
				field3.CheckConnectability();
				if (field3.GetStatus() & SolverStatus::INCONSISTENT) continue;

				if (current_progress < new_progress) transition = true;
				else {
					double trans_probability = exp((new_progress - current_progress) / temperature);
					if (rand() % 65536 < trans_probability * 65536) transition = true;
				}

				if (transition) {
					if (current_hint == SlitherlinkField::HINT_NONE) --number_of_unplaced_hints;
					current_progress = new_progress;
					is_progress = true;

					field.Init(field2);
					break;
				}
			}

			if (is_progress) break;
			else {
				current_problem.SetHint(i, j, previous_hint);
			}
		}

		if (field.GetStatus() == SolverStatus::SUCCESS && number_of_unplaced_hints == 0) break;

		if (!is_progress) {
			if (no_progress++ >= 20) return false;
		}
	}

	if (field.GetStatus() == SolverStatus::SUCCESS && number_of_unplaced_hints == 0) {
		ret.Init(height, width);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) if (field.GetHint(y, x) != SlitherlinkField::HINT_NONE) {
				ret.SetHint(y, x, field.GetHint(y, x));
			}
		}
		return true;
	}
	
	return false;
}

void SlitherlinkGenerator::SimplifyProblem(SlitherlinkProblem &problem, bool symmetry)
{
	int height = problem.GetHeight(), width = problem.GetWidth();

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (problem.GetHint(i, j) == SlitherlinkField::HINT_NONE) continue;

			if (!symmetry) {
				int hint_tmp = problem.GetHint(i, j);
				problem.SetHint(i, j, SlitherlinkField::HINT_NONE);

				SlitherlinkField field;
				field.Init(problem);
				field.Assume();

				if (field.GetStatus() != SolverStatus::SUCCESS) {
					problem.SetHint(i, j, hint_tmp);
				}
			} else {
				int i2 = height - 1 - i, j2 = width - 1 - j;
				int hint_tmp = problem.GetHint(i, j);
				int hint_tmp2 = problem.GetHint(i2, j2);
				problem.SetHint(i, j, SlitherlinkField::HINT_NONE);
				problem.SetHint(i2, j2, SlitherlinkField::HINT_NONE);

				SlitherlinkField field;
				field.Init(problem);
				field.Assume();

				if (field.GetStatus() != SolverStatus::SUCCESS) {
					problem.SetHint(i, j, hint_tmp);
					problem.SetHint(i2, j2, hint_tmp2);
				}
			}
		}
	}
}

}
