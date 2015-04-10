#include <ctime>
#include <vector>

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

bool SlitherlinkGenerator::GenerateOfShape(int height, int width, int *shape, SlitherlinkProblem &ret)
{
	SlitherlinkField field;
	field.Init(height, width);

	std::vector<std::pair<std::pair<int, int>, int> > current_hints;

	for (;;) {
		// put a hint randomly
		printf("%d\n", current_hints.size());
		std::vector<std::pair<std::pair<int, int>, int> > cand;
		bool exist_valid_cell = false;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				if (field.GetHint(i, j) != SlitherlinkField::HINT_NONE) continue;
				if (!shape[i * width + j]) continue;

				exist_valid_cell = true;
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

		if (!exist_valid_cell) break;
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

		continue;

	}

	if (field.GetStatus() == SolverStatus::SUCCESS) {
		ret.Init(height, width);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) if (field.GetHint(y, x) != SlitherlinkField::HINT_NONE) {
				ret.SetHint(y, x, field.GetHint(y, x));
			}
		}
		return true;
	}

	SlitherlinkProblem current_problem;
	current_problem.Init(height, width);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) if (field.GetHint(y, x) != SlitherlinkField::HINT_NONE) {
			current_problem.SetHint(y, x, field.GetHint(y, x));
		}
	}

	double valid_gap = -1.0;
	int best_score = 0;

	for (;;) {
		int current_progress = 0;
		bool is_progress = false;
		SlitherlinkField field;
		field.Init(current_problem);
		field.Assume();
		current_progress = field.GetProgress();
		if (best_score < current_progress) best_score = current_progress;

		std::vector<std::pair<int, int> > locs;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) if (current_problem.GetHint(i, j) != SlitherlinkField::HINT_NONE) {
				locs.push_back(std::make_pair(i, j));
			}
		}

		for (int i = 0; i < locs.size(); ++i) {
			int j = i + rand() % (locs.size() - i);
			if (i != j) std::swap(locs[i], locs[j]);
		}

		for (auto &t : locs) {
			int i = t.first, j = t.second;
			int current_hint = current_problem.GetHint(i, j);

			bool zero_validity = current_hints.size() >= 3;
			if (field.GetHintSafe(i - 1, j - 1) == 0
				|| field.GetHintSafe(i - 1, j) == 0
				|| field.GetHintSafe(i - 1, j + 1) == 0
				|| field.GetHintSafe(i, j - 1) == 0
				|| field.GetHintSafe(i, j + 1) == 0
				|| field.GetHintSafe(i + 1, j - 1) == 0
				|| field.GetHintSafe(i + 1, j) == 0
				|| field.GetHintSafe(i + 1, j + 1) == 0) zero_validity = false;

			for (int n = zero_validity ? 0 : 1; n <= 3; ++n) if (n != current_hint) {
				current_problem.SetHint(i, j, n);
				SlitherlinkField field2;
				field2.Init(current_problem);
				field2.Assume();

				if (!(field2.GetStatus() & SolverStatus::INCONSISTENT) && current_progress + 0 < field2.GetProgress()) {
					current_progress = field2.GetProgress();
					valid_gap += 0.1;
					if (valid_gap > 0) valid_gap = 0;
					printf("%d %f %d\n", current_progress, valid_gap, field2.GetProgress());
					is_progress = true;
					break;
				} else {
					current_problem.SetHint(i, j, current_hint);
				}
			}

			if (is_progress) break;
		}

		if (!is_progress) break;
	}

	field.Init(current_problem);
	field.Assume();

	if (field.GetStatus() == SolverStatus::SUCCESS) {
		ret.Init(height, width);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) if (field.GetHint(y, x) != SlitherlinkField::HINT_NONE) {
				ret.SetHint(y, x, field.GetHint(y, x));
			}
		}
		return true;
	}

	field.Debug();

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
