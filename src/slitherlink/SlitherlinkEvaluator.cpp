#include "SlitherlinkEvaluator.h"
#include "SlitherlinkMethod.hpp"
#include "../common/MiniVector.hpp"
#include "../common/UnionFind.h"
#include <set>

namespace Penciloid
{
void SlitherlinkEvaluator::Init(SlitherlinkProblem &problem)
{
	SlitherlinkMethod method;
	method.check_cell = method.adjacent_3 = method.diagonal_3 = method.diagonal_chain = false;
	method.grid_loop_method.already_two_lines = method.grid_loop_method.avoid_cycle = false;
	method.grid_loop_method.hourglass_level1 = method.grid_loop_method.hourglass_level2 = false;

	field.SetMethod(method);
	field.Init(problem, false);
}

double SlitherlinkEvaluator::Evaluate()
{
	double score = 0.0;

	double last_y = 1e8, last_x = 1e8;

	while (field.GetStatus() == SolverStatus::NORMAL) {
		std::vector<move> valid_moves;
		EnumerateValidMoves(valid_moves);

		if (valid_moves.size() == 0) {
			// field.Debug();
			return SCORE_IMPOSSIBLE;
		}

		// modify the score according to the locality
		for (move &m : valid_moves) {
			double locality_weight = 0.0;
			for (int i = 0; i < m.xs.size(); ++i) {
				locality_weight += std::min(1.0, (std::max(abs(last_y - m.ys[i]), abs(last_y - m.ys[i])) - 1) / 4.0);
			}
			locality_weight = pow(param.locality_base, locality_weight / m.xs.size() - 1);
			m.difficulty *= locality_weight;
		}

		// calculate the score of this move
		double current_score = 0.0;
		bool score_zero = false;
		for (move &m : valid_moves) {
			if (m.difficulty < 1e-6) {
				score_zero = true;
				break;
			} else {
				current_score += pow(m.difficulty / m.xs.size(), -param.alternative_dimension);
			}
		}

		if (score_zero) current_score = 0.0;
		else {
			current_score = pow(current_score, -1.0 / param.alternative_dimension);
		}
		current_score *= sqrt((field.GetHeight() + 1) * field.GetWidth() + (field.GetWidth() + 1) * field.GetHeight() - field.GetProgress());

		// choose a move and apply it
		int easiest_move = 0; double easiest_score = 1e5;

		for (int i = 0; i < valid_moves.size(); ++i) {
			double score_this_move = valid_moves[i].difficulty / valid_moves[i].xs.size();

			if (easiest_score > score_this_move) {
				easiest_score = score_this_move;
				easiest_move = i;
			}
		}

		move &m = valid_moves[easiest_move];
		score += current_score * m.xs.size();
		last_x = last_y = 0;

		for (int i = 0; i < m.xs.size(); ++i) {
			last_x += m.xs[i]; last_y += m.ys[i];
			if (m.styles[i] == LOOP_LINE) field.DetermineLine(m.ys[i], m.xs[i]);
			else if (m.styles[i] == LOOP_BLANK) field.DetermineBlank(m.ys[i], m.xs[i]);
		}
		last_x /= m.xs.size(); last_y /= m.xs.size();
	}

	if (field.GetStatus() != SolverStatus::SUCCESS) return SCORE_INCONSISTENT;

	return score;
}

void SlitherlinkEvaluator::EnumerateValidMoves(std::vector<move> &moves)
{
	CheckTwoLinesRule(moves);
	CheckAvoidCycleRule(moves);
	CheckTheoremsAbout3(moves);

	for (int y = 0; y <= field.GetHeight(); ++y) {
		for (int x = 0; x <= field.GetWidth(); ++x) {
			CheckHourglassRule(y * 2, x * 2, moves);
		}
	}
	for (int y = 0; y < field.GetHeight(); ++y) {
		for (int x = 0; x < field.GetWidth(); ++x) {
			if (!CheckAdjacentLinesRule(y, x, moves)) {
				// apply other rules
				CheckCornerCell(y, x, moves);
				CheckLineToClue(y, x, moves);
				CheckLineFromClue(y, x, moves);
				CheckDiagonalChain(y, x, moves);
			}
		}
	}

	CheckInOutRule(moves);

	EliminateDoneMoves(moves);
}

void SlitherlinkEvaluator::EliminateDoneMoves(std::vector<move> &moves)
{
	std::vector<move> valid_moves;

	for (move &m : moves) {
		move m2(m.difficulty);

		for (int i = 0; i < m.xs.size(); ++i) {
			if (field.GetSegmentStyleSafe(m.ys[i], m.xs[i]) == LOOP_UNDECIDED) {
				m2.ys.push_back(m.ys[i]);
				m2.xs.push_back(m.xs[i]);
				m2.styles.push_back(m.styles[i]);
			}
		}

		if (m2.ys.size() > 0) valid_moves.push_back(m2);
	}

	moves.swap(valid_moves);
}

void SlitherlinkEvaluator::CheckTwoLinesRule(std::vector<SlitherlinkEvaluator::move> &moves)
{
	for (int y = 0; y <= field.GetHeight() * 2; y += 2) {
		for (int x = 0; x <= field.GetWidth() * 2; x += 2) {
			int n_lines = 0;

			for (int i = 0; i < 4; ++i) {
				int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];
				int style = field.GetSegmentStyleSafe(y2, x2);

				if (style == LOOP_LINE) ++n_lines;
			}

			if (n_lines == 2) {
				for (int i = 0; i < 4; ++i) {
					int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];
					int style = field.GetSegmentStyleSafe(y2, x2);

					if (style == LOOP_UNDECIDED) {
						moves.push_back(move(y2, x2, LOOP_BLANK, param.two_lines));
					}
				}
			}
		}
	}
}

void SlitherlinkEvaluator::CheckAvoidCycleRule(std::vector<move> &moves)
{
	for (int y = 0; y <= field.GetHeight() * 2; y += 2) {
		for (int x = 0; x <= field.GetWidth() * 2; x += 2) {
			int line_destination = -1, line_weight = -1;

			for (int i = 0; i < 4; ++i) {
				int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];
				int style = field.GetSegmentStyleSafe(y2, x2);

				if (style == LOOP_LINE) {
					if (line_destination != -1) line_destination = -2;
					else {
						line_destination = field.GetSegmentDestination(y2, x2, y, x);
						line_weight = field.GetSegmentGroupSize(y2, x2);
					}
				}
			}

			if (line_destination < 0 || line_weight == field.GetTotalLines()) continue;

			for (int i = 0; i < 4; ++i) {
				int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];
				int style = field.GetSegmentStyleSafe(y2, x2);

				if (style == LOOP_UNDECIDED) {
					if (field.GetSegmentDestination(y2, x2, y, x) == line_destination) {
						moves.push_back(move(y2, x2, LOOP_BLANK, 1.0 /* TODO */));
					}
				}
			}
		}
	}
}

void SlitherlinkEvaluator::CheckHourglassRule(int y, int x, std::vector<move> &moves)
{
	MiniVector<std::pair<int, int>, 4> line, undecided;
	int line_weight = 0;

	for (int d = 0; d < 4; ++d) {
		int style = field.GetSegmentStyleSafe(y + GridConstant::GRID_DY[d], x + GridConstant::GRID_DX[d]);
		if (style == LOOP_LINE) {
			line.push_back(std::make_pair(y + GridConstant::GRID_DY[d], x + GridConstant::GRID_DX[d]));
			line_weight = field.GetSegmentGroupSize(y + GridConstant::GRID_DY[d], x + GridConstant::GRID_DX[d]);
		} else if (style == LOOP_UNDECIDED) {
			undecided.push_back(std::make_pair(y + GridConstant::GRID_DY[d], x + GridConstant::GRID_DX[d]));
		}
	}
	if (!(line.size() == 1 && undecided.size() == 2)) return;

	int line_companion = field.GetSegmentDestination(line[0].first, line[0].second, y, x);
	for (int t_ = 0; t_ < 2; ++t_) {
		int target = field.GetSegmentDestination(undecided[t_].first, undecided[t_].second, y, x);
		int ty = field.SegmentY(target), tx = field.SegmentX(target);
		int line_weight2 = line_weight;
		bool there_triangle = false;

		for (int d = 0; d < 4; ++d) {
			if (field.GetSegmentStyleSafe(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d]) == LOOP_LINE) {
				// check if there is a 'triangle'?
				if (field.GetSegmentDestination(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d], ty, tx) ==
					field.GetSegmentDestination(undecided[1 - t_].first, undecided[1 - t_].second, y, x)) {
					line_weight2 += field.GetSegmentGroupSize(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d]);
					there_triangle = true;
				} else return;
			}
		}
		if (!there_triangle || line_weight2 >= field.GetTotalLines()) return; // no other 'line' segments

		for (int d = 0; d < 4; ++d) {
			if (field.GetSegmentStyleSafe(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d]) == LOOP_UNDECIDED) {
				if (field.GetSegmentDestination(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d], ty, tx) == line_companion) {
					// TODO: set the difficulty more carefully
					moves.push_back(move(ty + GridConstant::GRID_DY[d], tx + GridConstant::GRID_DX[d], LOOP_BLANK, 4.0));
				}
			}
		}
	}
}

void SlitherlinkEvaluator::CheckTheoremsAbout3(std::vector<move> &moves)
{
	for (int y = 0; y < field.GetHeight(); ++y) {
		for (int x = 0; x < field.GetWidth(); ++x) if (field.GetClue(y, x) == 3) {
			if (y != field.GetHeight() - 1 && field.GetClue(y + 1, x) == 3) {
				move m(param.adjacent_3);
				m.add(y * 2 + 0, x * 2 + 1, LOOP_LINE);
				m.add(y * 2 + 2, x * 2 + 1, LOOP_LINE);
				m.add(y * 2 + 4, x * 2 + 1, LOOP_LINE);
				m.add(y * 2 + 2, x * 2 - 1, LOOP_BLANK);
				m.add(y * 2 + 2, x * 2 + 3, LOOP_BLANK);

				moves.push_back(m);
			}
			if (x != field.GetWidth() - 1 && field.GetClue(y, x + 1) == 3) {
				move m(param.adjacent_3);
				m.add(y * 2 + 1, x * 2 + 0, LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 2, LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 4, LOOP_LINE);
				m.add(y * 2 - 1, x * 2 + 2, LOOP_BLANK);
				m.add(y * 2 + 3, x * 2 + 2, LOOP_BLANK);

				moves.push_back(m);
			}

			if (y != field.GetHeight() - 1 && x != field.GetWidth() - 1 && field.GetClue(y + 1, x + 1) == 3) {
				move m(param.diagonal_3);
				m.add(y * 2 + 0, x * 2 + 1, LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 0, LOOP_LINE);
				m.add(y * 2 + 3, x * 2 + 4, LOOP_LINE);
				m.add(y * 2 + 4, x * 2 + 3, LOOP_LINE);

				moves.push_back(m);
			}
			if (y != field.GetHeight() - 1 && x != 0 && field.GetClue(y + 1, x - 1) == 3) {
				move m(param.diagonal_3);
				m.add(y * 2 + 1, x * 2 + 2, LOOP_LINE);
				m.add(y * 2 + 0, x * 2 + 1, LOOP_LINE);
				m.add(y * 2 + 3, x * 2 - 2, LOOP_LINE);
				m.add(y * 2 + 4, x * 2 - 1, LOOP_LINE);

				moves.push_back(m);
			}
		}
	}
}

bool SlitherlinkEvaluator::CheckAdjacentLinesRule(int y, int x, std::vector<move> &moves)
{
	// This function returns whether this rule was successfully applied (or nothing is to be applied)

	if (field.GetClue(y, x) == SlitherlinkField::CLUE_NONE) return false;

	int clue = field.GetClue(y, x), n_lines = 0, n_blanks = 0;

	for (int i = 0; i < 4; ++i) {
		int style = field.GetSegmentStyle(y * 2 + 1 + GridConstant::GRID_DY[i], x * 2 + 1 + GridConstant::GRID_DX[i]);

		if (style == LOOP_LINE) ++n_lines;
		else if (style == LOOP_BLANK) ++n_blanks;
	}

	if (n_lines == clue) {
		move m(param.adjacent_lines[clue]);

		for (int i = 0; i < 4; ++i) {
			int y2 = y * 2 + 1 + GridConstant::GRID_DY[i], x2 = x * 2 + 1 + GridConstant::GRID_DX[i];

			if (field.GetSegmentStyle(y2, x2) == LOOP_UNDECIDED) {
				// moves.push_back(move(y2, x2, LOOP_BLANK, DIFFICULTY_ADJACENT_LINES[clue]));
				m.add(y2, x2, LOOP_BLANK);
			}
		}
		moves.push_back(m);
	}
	if (4 - n_blanks == clue) {
		move m(param.adjacent_lines[clue]);

		for (int i = 0; i < 4; ++i) {
			int y2 = y * 2 + 1 + GridConstant::GRID_DY[i], x2 = x * 2 + 1 + GridConstant::GRID_DX[i];

			if (field.GetSegmentStyle(y2, x2) == LOOP_UNDECIDED) {
				// moves.push_back(move(y2, x2, LOOP_LINE, DIFFICULTY_ADJACENT_LINES[clue]));
				m.add(y2, x2, LOOP_LINE);
			}
		}
		moves.push_back(m);
	}
	return n_lines == clue || 4 - n_blanks == clue;
}

void SlitherlinkEvaluator::CheckCornerCell(int y, int x, std::vector<move> &moves)
{
	int clue = field.GetClue(y, x);
	if (clue == SlitherlinkField::CLUE_NONE) return;

	for (int i = 0; i < 4; ++i) {
		int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
		int dy2 = GridConstant::GRID_DY[(i + 1) % 4], dx2 = GridConstant::GRID_DX[(i + 1) % 4];
		
		if (field.GetSegmentStyleSafe(y * 2 + 1 + dy1 * 2 + dy2, x * 2 + 1 + dx1 * 2 + dx2) == LOOP_BLANK &&
			field.GetSegmentStyleSafe(y * 2 + 1 + dy2 * 2 + dy1, x * 2 + 1 + dx2 * 2 + dx1) == LOOP_BLANK) {
			if (clue == 1) {
				move m(param.corner_clue[1]);
				m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, LOOP_BLANK);
				m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, LOOP_BLANK);
				moves.push_back(m);
			}
			if (clue == 2) {
				{
					move m(param.corner_clue[2]);

					if (field.GetSegmentStyleSafe(y * 2 + 1 + dy1, x * 2 + 1 + dx1) == LOOP_LINE ||
						field.GetSegmentStyleSafe(y * 2 + 1 + dy2, x * 2 + 1 + dx2) == LOOP_LINE ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_BLANK ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_BLANK ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy1 * 2 - dy2, x * 2 + 1 - dx1 * 2 - dx2) == LOOP_LINE ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy2 * 2 - dy1, x * 2 + 1 - dx2 * 2 - dx1) == LOOP_LINE) {
						m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, LOOP_LINE);
						m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, LOOP_LINE);
						m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_BLANK);
						m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_BLANK);
					}
					if (field.GetSegmentStyleSafe(y * 2 + 1 + dy1, x * 2 + 1 + dx1) == LOOP_BLANK ||
						field.GetSegmentStyleSafe(y * 2 + 1 + dy2, x * 2 + 1 + dx2) == LOOP_BLANK ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_LINE ||
						field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_LINE) {
						m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, LOOP_BLANK);
						m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, LOOP_BLANK);
						m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_LINE);
						m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_LINE);
					}

					moves.push_back(m);
				}

				{
					move m(param.corner_clue[2]);

					for (int t = 0; t < 2; ++t) {
						int sgn = (t == 0 ? 1 : -1);

						if (field.GetSegmentStyleSafe(y * 2 + 1 + sgn * (dy1 * 2 - dy2), x * 2 + 1 + sgn * (dx1 * 2 - dx2)) == LOOP_BLANK) {
							m.add(y * 2 + 1 + sgn * (dy1 - dy2 * 2), x * 2 + 1 + sgn * (dx1 - dx2 * 2), LOOP_LINE);
						}
						if (field.GetSegmentStyleSafe(y * 2 + 1 + sgn * (dy1 * 2 - dy2), x * 2 + 1 + sgn * (dx1 * 2 - dx2)) == LOOP_LINE) {
							m.add(y * 2 + 1 + sgn * (dy1 - dy2 * 2), x * 2 + 1 + sgn * (dx1 - dx2 * 2), LOOP_BLANK);
						}
						if (field.GetSegmentStyleSafe(y * 2 + 1 + sgn * (dy2 * 2 - dy1), x * 2 + 1 + sgn * (dx2 * 2 - dx1)) == LOOP_BLANK) {
							m.add(y * 2 + 1 + sgn * (dy2 - dy1 * 2), x * 2 + 1 + sgn * (dx2 - dx1 * 2), LOOP_LINE);
						}
						if (field.GetSegmentStyleSafe(y * 2 + 1 + sgn * (dy2 * 2 - dy1), x * 2 + 1 + sgn * (dx2 * 2 - dx1)) == LOOP_LINE) {
							m.add(y * 2 + 1 + sgn * (dy2 - dy1 * 2), x * 2 + 1 + sgn * (dx2 - dx1 * 2), LOOP_BLANK);
						}
					}

					moves.push_back(m);
				}
			}
			if (clue == 3) {
				move m(param.corner_clue[3]);
				m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, LOOP_LINE);
				m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, LOOP_LINE);
				moves.push_back(m);
			}
		}
	}
}

void SlitherlinkEvaluator::CheckLineToClue(int y, int x, std::vector<move> &moves)
{
	int clue = field.GetClue(y, x);
	if (clue == SlitherlinkField::CLUE_NONE) return;

	for (int i = 0; i < 4; ++i) {
		int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
		int dy2 = GridConstant::GRID_DY[(i + 1) % 4], dx2 = GridConstant::GRID_DX[(i + 1) % 4];

		int in_y1 = y * 2 + 1 + dy1 * 2 + dy2, in_x1 = x * 2 + 1 + dx1 * 2 + dx2;
		int in_y2 = y * 2 + 1 + dy2 * 2 + dy1, in_x2 = x * 2 + 1 + dx2 * 2 + dx1;

		if (clue == 1) {
			if ((field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE && field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_BLANK) || 
				(field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE && field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_BLANK)) {
				move m(param.line_to_clue[1]);

				m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_BLANK);
				m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_BLANK);
				moves.push_back(m);
			}
		}

		if (clue == 2) {
			bool is_applicable = false;

			if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE && field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_BLANK) is_applicable = true;
			if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE && field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_BLANK) is_applicable = true;
			if (field.GetSegmentStyleSafe(y * 2 + 1 + dy1, x * 2 + 1 + dx1) == LOOP_LINE && field.GetSegmentStyleSafe(y * 2 + 1 + dy2, x * 2 + 1 + dx2))
				is_applicable = true;
			if (field.GetSegmentStyleSafe(y * 2 + 1 + dy2, x * 2 + 1 + dx2) == LOOP_LINE && field.GetSegmentStyleSafe(y * 2 + 1 + dy1, x * 2 + 1 + dx1))
				is_applicable = true;

			if (!is_applicable) continue;

			move m(param.line_to_clue[2]);

			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_LINE) m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_BLANK);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_BLANK) m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_LINE);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_LINE) m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_BLANK);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_BLANK) m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_LINE);

			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1 * 2 - dy2, x * 2 + 1 - dx1 * 2 - dx2) == LOOP_LINE) m.add(y * 2 + 1 - dy2 * 2 - dy1, x * 2 + 1 - dx2 * 2 - dx1, LOOP_BLANK);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1 * 2 - dy2, x * 2 + 1 - dx1 * 2 - dx2) == LOOP_BLANK) m.add(y * 2 + 1 - dy2 * 2 - dy1, x * 2 + 1 - dx2 * 2 - dx1, LOOP_LINE);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy2 * 2 - dy1, x * 2 + 1 - dx2 * 2 - dx1) == LOOP_LINE) m.add(y * 2 + 1 - dy1 * 2 - dy2, x * 2 + 1 - dx1 * 2 - dx2, LOOP_BLANK);
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy2 * 2 - dy1, x * 2 + 1 - dx2 * 2 - dx1) == LOOP_BLANK) m.add(y * 2 + 1 - dy1 * 2 - dy2, x * 2 + 1 - dx1 * 2 - dx2, LOOP_LINE);

			moves.push_back(m);
		}

		if (clue == 3) {
			if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE ||
				field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE) {
				move m(param.line_to_clue[3]);

				if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE) m.add(in_y2, in_x2, LOOP_BLANK);
				if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE) m.add(in_y1, in_x1, LOOP_BLANK);
				m.add(y * 2 + 1 - dy1, x * 2 + 1 - dx1, LOOP_LINE);
				m.add(y * 2 + 1 - dy2, x * 2 + 1 - dx2, LOOP_LINE);
				moves.push_back(m);
			}
		}
	}
}

void SlitherlinkEvaluator::CheckLineFromClue(int y, int x, std::vector<move> &moves)
{
	int clue = field.GetClue(y, x);
	if (clue == SlitherlinkField::CLUE_NONE) return;

	for (int i = 0; i < 4; ++i) {
		int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
		int dy2 = GridConstant::GRID_DY[(i + 1) % 4], dx2 = GridConstant::GRID_DX[(i + 1) % 4];

		int in_y1 = y * 2 + 1 + dy1 * 2 + dy2, in_x1 = x * 2 + 1 + dx1 * 2 + dx2;
		int in_y2 = y * 2 + 1 + dy2 * 2 + dy1, in_x2 = x * 2 + 1 + dx2 * 2 + dx1;

		if (clue == 1) {
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_BLANK &&
				field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_BLANK) {
				move m(param.line_from_clue[1]);

				if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE)
					m.add(in_y2, in_x2, LOOP_BLANK);
				if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_BLANK)
					m.add(in_y2, in_x2, LOOP_LINE);
				if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE)
					m.add(in_y1, in_x1, LOOP_BLANK);
				if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_BLANK)
					m.add(in_y1, in_x1, LOOP_LINE);

				moves.push_back(m);
			}
		}

		if (clue == 3) {
			if (field.GetSegmentStyleSafe(y * 2 + 1 - dy1, x * 2 + 1 - dx1) == LOOP_LINE &&
				field.GetSegmentStyleSafe(y * 2 + 1 - dy2, x * 2 + 1 - dx2) == LOOP_LINE) {
				move m(param.line_from_clue[3]);

				if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_LINE)
					m.add(in_y2, in_x2, LOOP_BLANK);
				if (field.GetSegmentStyleSafe(in_y1, in_x1) == LOOP_BLANK)
					m.add(in_y2, in_x2, LOOP_LINE);
				if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_LINE)
					m.add(in_y1, in_x1, LOOP_BLANK);
				if (field.GetSegmentStyleSafe(in_y2, in_x2) == LOOP_BLANK)
					m.add(in_y1, in_x1, LOOP_LINE);

				moves.push_back(m);
			}
		}
	}
}

void SlitherlinkEvaluator::CheckDiagonalChain(int y, int x, std::vector<move> &moves)
{
	int y0 = y, x0 = x;

	for (int dir = 0; dir < 4; ++dir) {
		int dy = GridConstant::GRID_DY[dir] + GridConstant::GRID_DY[(dir + 1) % 4];
		int dx = GridConstant::GRID_DX[dir] + GridConstant::GRID_DX[(dir + 1) % 4];

		y = y0 * 2 + 1; x = x0 * 2 + 1;
		int cnt = -1;
		if (field.GetSegmentStyleSafe(y + dy, x) != LOOP_UNDECIDED && field.GetSegmentStyleSafe(y, x + dx) != LOOP_UNDECIDED) {
			cnt = ((field.GetSegmentStyleSafe(y + dy, x) == LOOP_LINE ? 1 : 0) + (field.GetSegmentStyleSafe(y, x + dx) == LOOP_LINE ? 1 : 0)) % 2;
		}
		if (field.GetSegmentStyleSafe(y - dy, x) != LOOP_UNDECIDED && field.GetSegmentStyleSafe(y, x - dx) != LOOP_UNDECIDED && field.GetClue(y / 2, x / 2) != SlitherlinkField::CLUE_NONE) {
			cnt = ((field.GetSegmentStyleSafe(y - dy, x) == LOOP_LINE ? 1 : 0) + (field.GetSegmentStyleSafe(y, x - dx) == LOOP_LINE ? 1 : 0) + field.GetClue(y / 2, x / 2)) % 2;
		}
		if (field.GetSegmentStyleSafe(y - dy, x - dx * 2) != LOOP_UNDECIDED && field.GetSegmentStyleSafe(y - dy * 2 , x - dx) != LOOP_UNDECIDED && field.GetClue(y / 2, x / 2) != SlitherlinkField::CLUE_NONE) {
			cnt = ((field.GetSegmentStyleSafe(y - dy, x - dx * 2) == LOOP_LINE ? 1 : 0) + (field.GetSegmentStyleSafe(y - dy * 2, x - dx) == LOOP_LINE ? 1 : 0) + field.GetClue(y / 2, x / 2)) % 2;
		}

		if (cnt == -1) continue;

		for (;;) {
			y += dy * 2; x += dx * 2;
			if (!field.IsProperCoordinate(y, x)) break;

			int clue = field.GetClue(y / 2, x / 2);

			if (clue == 2) {
				move m(param.diagonal_chain);

				if (field.GetSegmentStyleSafe(y + dy, x) != LOOP_UNDECIDED) {
					m.add(y, x + dx, ((field.GetSegmentStyleSafe(y + dy, x) == LOOP_LINE) ^ (cnt == 1)) ? LOOP_LINE : LOOP_BLANK);
				}
				if (field.GetSegmentStyleSafe(y, x + dx) != LOOP_UNDECIDED) {
					m.add(y + dy, x, ((field.GetSegmentStyleSafe(y, x + dx) == LOOP_LINE) ^ (cnt == 1)) ? LOOP_LINE : LOOP_BLANK);
				}
				if (field.GetSegmentStyleSafe(y + dy, x + 2 * dx) != LOOP_UNDECIDED) {
					m.add(y + 2 * dy, x + dx, ((field.GetSegmentStyleSafe(y + dy, x + 2 * dx) == LOOP_LINE) ^ (cnt == 1)) ? LOOP_LINE : LOOP_BLANK);
				}
				if (field.GetSegmentStyleSafe(y + 2 * dy, x + dx) != LOOP_UNDECIDED) {
					m.add(y + dy, x + 2 * dx, ((field.GetSegmentStyleSafe(y + 2 * dy, x + dx) == LOOP_LINE) ^ (cnt == 1)) ? LOOP_LINE : LOOP_BLANK);
				}

				moves.push_back(m);

				if (m.xs.size() > 0) break;
				continue;
			}

			if (clue == 1) {
				move m(param.diagonal_chain);

				if (cnt == 0) {
					m.add(y - dy, x, LOOP_BLANK);
					m.add(y, x - dx, LOOP_BLANK);
				} else if (cnt == 1) {
					m.add(y + dy, x, LOOP_BLANK);
					m.add(y, x + dx, LOOP_BLANK);
				}

				moves.push_back(m);
			}
			if (clue == 3) {
				move m(param.diagonal_chain);

				if (cnt == 0) {
					m.add(y - dy, x, LOOP_LINE);
					m.add(y, x - dx, LOOP_LINE);
				} else if (cnt == 1) {
					m.add(y + dy, x, LOOP_LINE);
					m.add(y, x + dx, LOOP_LINE);
				}

				moves.push_back(m);
			}
			break;
		}
	}
}

void SlitherlinkEvaluator::CheckInOutRule(std::vector<move> &moves)
{
	UnionFind uf(field.GetHeight() * field.GetWidth() * 2 + 2);
	int out_of_grid = field.GetHeight() * field.GetWidth();

	for (int y = 0; y <= 2 * field.GetHeight(); ++y) {
		for (int x = 0; x <= 2 * field.GetWidth(); ++x) {
			if (y % 2 == 1 && x % 2 == 0) {
				int cell1 = field.IsProperCoordinate(y, x - 1) ? ((y / 2) * field.GetWidth() + (x / 2 - 1)) : out_of_grid;
				int cell2 = field.IsProperCoordinate(y, x + 1) ? ((y / 2) * field.GetWidth() + (x / 2)) : out_of_grid;

				if (field.GetSegmentStyle(y, x) == LOOP_LINE) {
					uf.Join(cell1 * 2, cell2 * 2 + 1);
					uf.Join(cell1 * 2 + 1, cell2 * 2);
				} else if (field.GetSegmentStyle(y, x) == LOOP_BLANK) {
					uf.Join(cell1 * 2, cell2 * 2);
					uf.Join(cell1 * 2 + 1, cell2 * 2 + 1);
				}
			} else if (y % 2 == 0 && x % 2 == 1) {
				int cell1 = field.IsProperCoordinate(y - 1, x) ? ((y / 2 - 1) * field.GetWidth() + (x / 2)) : out_of_grid;
				int cell2 = field.IsProperCoordinate(y + 1, x) ? ((y / 2) * field.GetWidth() + (x / 2)) : out_of_grid;

				if (field.GetSegmentStyle(y, x) == LOOP_LINE) {
					uf.Join(cell1 * 2, cell2 * 2 + 1);
					uf.Join(cell1 * 2 + 1, cell2 * 2);
				} else if (field.GetSegmentStyle(y, x) == LOOP_BLANK) {
					uf.Join(cell1 * 2, cell2 * 2);
					uf.Join(cell1 * 2 + 1, cell2 * 2 + 1);
				}
			}
		}
	}

	for (int y = 0; y <= field.GetHeight() * 2; ++y) {
		for (int x = 0; x <= field.GetWidth() * 2; ++x) {
			int cell1, cell2;

			if (y % 2 == 1 && x % 2 == 0) {
				cell1 = field.IsProperCoordinate(y, x - 1) ? ((y / 2) * field.GetWidth() + (x / 2 - 1)) : out_of_grid;
				cell2 = field.IsProperCoordinate(y, x + 1) ? ((y / 2) * field.GetWidth() + (x / 2)) : out_of_grid;
			} else if (y % 2 == 0 && x % 2 == 1) {
				cell1 = field.IsProperCoordinate(y - 1, x) ? ((y / 2 - 1) * field.GetWidth() + (x / 2)) : out_of_grid;
				cell2 = field.IsProperCoordinate(y + 1, x) ? ((y / 2) * field.GetWidth() + (x / 2)) : out_of_grid;
			} else continue;

			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2)) {
				if (field.IsRepresentative(y, x)) moves.push_back(move(y, x, LOOP_BLANK, 3.5 /* TODO */));
			}
			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2 + 1)) {
				if (field.IsRepresentative(y, x)) moves.push_back(move(y, x, LOOP_LINE, 3.5 /* TODO */));
			}
		}
	}

}

}