#include <ctime>
#include <vector>
#include <cmath>

#include "../common/XorShift.h"
#include "../common/UnionFind.h"
#include "SlitherlinkGenerator.h"
#include "SlitherlinkField.h"
#include "SlitherlinkProblem.h"
#include "SlitherlinkDatabase.h"
#include "SlitherlinkProblemConstraint.h"

static int FieldHash(Penciloid::SlitherlinkField &field, int hash_size)
{
	long long ret = 0;

	for (int i = 0; i < field.GetHeight(); ++i) {
		for (int j = 0; j < field.GetWidth(); ++j) {
			ret = (ret << 3) | (field.GetHint(i, j) + 1);
			ret %= hash_size;
		}
	}
	return (int)ret;
}

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

bool SlitherlinkGenerator::GenerateOfShape(SlitherlinkProblemConstraint &constraint, SlitherlinkProblem &ret, XorShift &rnd, bool use_assumption)
{
	int height = constraint.GetHeight(), width = constraint.GetWidth();
	const int hash_size = 100007;
	int hash[hash_size]; for (int i = 0; i < hash_size; ++i) hash[i] = 0;

	SlitherlinkProblem current_problem;
	current_problem.Init(height, width);

	int max_step = height * width * 10;
	int number_of_unplaced_hints = 0;
	int no_progress = 0;
	int moc = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int cont = constraint.GetCellConstraint(i, j);

			if (cont >= 0 && cont <= 3) current_problem.SetHint(i, j, cont);
			else if (cont == SlitherlinkProblemConstraint::HINT_SOME) ++number_of_unplaced_hints;
		}
	}

	SlitherlinkField field;
	field.Init(current_problem);
	for (int step = 0; step < max_step; ++step) {
		int current_progress = field.GetProgress();
		bool is_progress = false;

		double temperature = 5.0; // 7.0 * exp(-2.0 * (double)step / max_step);
		std::vector<std::pair<int, int> > locs;
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) if (constraint.GetCellConstraint(i, j) == SlitherlinkProblemConstraint::HINT_SOME) {
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
			int j = i + rnd.NextInt() % (locs.size() - i);
			if (i != j) std::swap(locs[i], locs[j]);
		}

		int trial = 0;
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
				int j = i + rnd.NextInt() % (nums.size() - i);
				if (i != j) std::swap(nums[i], nums[j]);
			}

			int previous_hint = current_problem.GetHint(i, j);
			SlitherlinkField common;

			if (previous_hint == SlitherlinkField::HINT_NONE) {
				common.Init(field);
			} else {
				current_problem.SetHint(i, j, SlitherlinkField::HINT_NONE);
				common.Init(current_problem, use_assumption);
			}

			for (int n : nums) {
				current_problem.SetHint(i, j, n);
				SlitherlinkField field2;
				field2.Init(common); ++trial;
				field2.SetHint(i, j, n);
				if (use_assumption) field2.Assume();

				bool transition = false;

				if (field2.GetStatus() & SolverStatus::INCONSISTENT) continue;

				int new_progress = field2.GetProgress();

				if (current_progress < new_progress) transition = true;
				else {
					double trans_probability = exp((new_progress - current_progress) / temperature);
					if (rnd.NextInt() % 65536 < trans_probability * 65536) transition = true;
				}
				if (!transition) continue;

				SlitherlinkField field3;
				field3.Init(field2);
				field3.CheckInOutRule();
				field3.CheckConnectability();
				if (field3.GetStatus() & SolverStatus::INCONSISTENT) continue;

				int hash_id = FieldHash(field2, hash_size);
				if (hash[hash_id] >= 10) continue;

				if (transition) {
					if (current_hint == SlitherlinkField::HINT_NONE) --number_of_unplaced_hints;
					moc = std::max(moc, ++hash[hash_id]);
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
		if (field.GetStatus() == SolverStatus::SUCCESS && number_of_unplaced_hints == 0) {
			break;
		}

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
void SlitherlinkGenerator::GenerateConstraint(int height, int width, int n_hints, int symmetricity, XorShift &rnd, SlitherlinkProblemConstraint &ret)
{
	UnionFind hint_group(height * width);
	ret.Init(height, width);

	if ((symmetricity & (SHAPE_ROTATION_90 ^ SHAPE_ROTATION_180)) && height == width) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < height; ++j) {
				int id = i * height + j;
				int rot90 = j * height + (height - 1 - i);

				hint_group.Join(id, rot90);
			}
		}
	} else if (symmetricity & SHAPE_ROTATION_180) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				int id = i * width + j;
				int rot180 = (height - 1 - i) * width + (width - 1 - j);

				hint_group.Join(id, rot180);
			}
		}
	}

	if (symmetricity & SHAPE_MIRROR_HORIZONTAL) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width / 2; ++j) {
				int id = i * width + j;
				int symm = i * width + (width - 1 - j);

				hint_group.Join(id, symm);
			}
		}
	}

	if (symmetricity & SHAPE_MIRROR_VERTICAL) {
		for (int i = 0; i < height / 2; ++i) {
			for (int j = 0; j < width; ++j) {
				int id = i * width + j;
				int symm = (height - 1 - i) * width + j;

				hint_group.Join(id, symm);
			}
		}
	}

	std::vector<std::pair<int, int> > possible_hints, candidates;

	for (int i = 0; i < height * width; ++i) if (hint_group.Root(i) == i)
		possible_hints.push_back(std::make_pair(i, hint_group.UnionSize(i)));

	while (n_hints > 0) {
		int score_sum = 0;
		candidates.clear();

		for (int i = 0; i < possible_hints.size(); ++i) if (possible_hints[i].second <= n_hints) {
			int y = possible_hints[i].first / width, x = possible_hints[i].first % width;

			if (ret.IsHint(y, x)) continue;

			int score = 0;

			for (int dy = -2; dy <= 2; ++dy) {
				for (int dx = -2; dx <= 2; ++dx) {
					int y2 = y + dy, x2 = x + dx;
					if (0 <= y2 && y2 < height && 0 <= x2 && x2 < width) {
						if (ret.IsHint(y2, x2)) {
							score += 5 - abs(dy) - abs(dx);
							if (abs(dy) + abs(dx) == 1) score += 2;
						}
					}
				}
			}

			score = (int)(64 * pow(2, (16 - score) / 2.0) + 4);
			score_sum += score;
			candidates.push_back(std::make_pair(i, score));
		}

		if (candidates.size() == 0) break;

		int cand_pos = (int)(rnd.NextInt() % score_sum);
		
		for (int i = 0; i < candidates.size(); ++i) {
			if (cand_pos < candidates[i].second) {
				int representative = possible_hints[candidates[i].first].first;

				// TODO: improve time complexity
				for (int j = 0; j < height * width; ++j) if (hint_group.Root(j) == hint_group.Root(representative)) {
					--n_hints;
					ret.SetCellConstraint(j / width, j % width, SlitherlinkProblemConstraint::HINT_SOME);
				}
				break;
			} else cand_pos -= candidates[i].second;
		}
	}
	for (int i = 0; i < height; ++i) for (int j = 0; j < width; ++j) if (ret.IsHint(i, j)) ++n_hints;
}

}
