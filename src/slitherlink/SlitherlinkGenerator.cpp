#include <ctime>
#include <vector>

#include "SlitherlinkGenerator.h"
#include "SlitherlinkField.h"
#include "SlitherlinkProblem.h"
#include "SlitherlinkDatabase.h"

namespace Penciloid
{
bool SlitherlinkGenerator::GenerateNaive(int height, int width, SlitherlinkProblem &ret)
{
	SlitherlinkDatabase::CreateDatabase();
	srand((int)time(NULL));

	SlitherlinkField field;
	field.Init(height, width);

	for (int s = 0;; ++s) {
		// put a hint randomly
		std::vector<std::pair<std::pair<int, int>, int> > cand;
		
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				if (field.GetHint(i, j) != SlitherlinkField::HINT_NONE) continue;

				bool zero_validity = s >= 3;
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
			break;
		}

		auto next_step = cand[rand() % cand.size()];
		field.SetHint(next_step.first.first, next_step.first.second, next_step.second);
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
	}

	return false;
}

}
