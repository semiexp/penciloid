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
	SlitherlinkDatabase::CreateDatabase();

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
