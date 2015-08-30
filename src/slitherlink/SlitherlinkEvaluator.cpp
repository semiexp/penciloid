#include "SlitherlinkEvaluator.h"
#include "SlitherlinkMethod.hpp"
#include "../common/MiniVector.hpp"

namespace Penciloid
{
const double SlitherlinkEvaluator::DIFFICULTY_TWO_LINES = 0.0;
const double SlitherlinkEvaluator::DIFFICULTY_ADJACENT_LINES[4] = { 1.0, 1.5, 1.5, 1.5 };
const double SlitherlinkEvaluator::DIFFICULTY_ADJACENT_3 = 3.0;
const double SlitherlinkEvaluator::DIFFICULTY_DIAGONAL_3 = 3.0;
const double SlitherlinkEvaluator::DIFFICULTY_CORNER_CLUE[4] = { 0.0, 2.0, 2.5, 2.0 };

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
	while (field.GetStatus() == SolverStatus::NORMAL) {
		std::vector<move> valid_moves;
		EnumerateValidMoves(valid_moves);

		if (valid_moves.size() == 0) {
			field.Debug();
			return SCORE_IMPOSSIBLE;
		}

		// calculate the score of this move
		double current_score = 0.0;
		bool score_zero = false;
		for (move &m : valid_moves) {
			if (m.difficulty < 1e-6) {
				score_zero = true;
				break;
			} else {
				current_score += pow(m.difficulty, -2.0);
			}
		}

		if (score_zero) current_score = 0.0;
		else {
			current_score = pow(current_score, -1.0 / 2.0);
		}
		score += current_score;

		// choose a move and apply it
		int easiest_move = 0;

		for (int i = 1; i < valid_moves.size(); ++i) {
			if (valid_moves[easiest_move].difficulty > valid_moves[i].difficulty) easiest_move = i;
		}

		move &m = valid_moves[easiest_move];
		for (int i = 0; i < m.xs.size(); ++i) {
			if (m.styles[i] == SlitherlinkField::LOOP_LINE) field.DetermineLine(m.ys[i], m.xs[i]);
			else if (m.styles[i] == SlitherlinkField::LOOP_BLANK) field.DetermineBlank(m.ys[i], m.xs[i]);
		}
	}

	if (field.GetStatus() != SolverStatus::SUCCESS) return SCORE_INCONSISTENT;

	return score;
}

void SlitherlinkEvaluator::EnumerateValidMoves(std::vector<move> &moves)
{
	CheckTwoLinesRule(moves);
	CheckAvoidCycleRule(moves);
	CheckTheoremsAbout3(moves);

	for (int y = 0; y < field.GetHeight(); ++y) {
		for (int x = 0; x < field.GetWidth(); ++x) {
			if (!CheckAdjacentLinesRule(y, x, moves)) {
				// apply other rules
				CheckCornerCell(y, x, moves);
			}
		}
	}

	EliminateDoneMoves(moves);
}

void SlitherlinkEvaluator::EliminateDoneMoves(std::vector<move> &moves)
{
	std::vector<move> valid_moves;

	for (move &m : moves) {
		move m2(m.difficulty);

		for (int i = 0; i < m.xs.size(); ++i) {
			if (field.GetSegmentStyleSafe(m.ys[i], m.xs[i]) == SlitherlinkField::LOOP_UNDECIDED) {
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

				if (style == SlitherlinkField::LOOP_LINE) ++n_lines;
			}

			if (n_lines == 2) {
				for (int i = 0; i < 4; ++i) {
					int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];
					int style = field.GetSegmentStyleSafe(y2, x2);

					if (style == SlitherlinkField::LOOP_UNDECIDED) {
						moves.push_back(move(y2, x2, SlitherlinkField::LOOP_BLANK, DIFFICULTY_TWO_LINES));
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

				if (style == SlitherlinkField::LOOP_LINE) {
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

				if (style == SlitherlinkField::LOOP_UNDECIDED) {
					if (field.GetSegmentDestination(y2, x2, y, x) == line_destination) {
						moves.push_back(move(y2, x2, SlitherlinkField::LOOP_BLANK, 1.0 /* TODO */));
					}
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
				move m(DIFFICULTY_ADJACENT_3);
				m.add(y * 2 + 0, x * 2 + 1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 2, x * 2 + 1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 4, x * 2 + 1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 2, x * 2 - 1, SlitherlinkField::LOOP_BLANK);
				m.add(y * 2 + 2, x * 2 + 3, SlitherlinkField::LOOP_BLANK);

				moves.push_back(m);
			}
			if (x != field.GetWidth() - 1 && field.GetClue(y, x + 1) == 3) {
				move m(DIFFICULTY_ADJACENT_3);
				m.add(y * 2 + 1, x * 2 + 0, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 2, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 4, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 - 1, x * 2 + 2, SlitherlinkField::LOOP_BLANK);
				m.add(y * 2 + 3, x * 2 + 2, SlitherlinkField::LOOP_BLANK);

				moves.push_back(m);
			}

			if (y != field.GetHeight() - 1 && x != field.GetWidth() - 1 && field.GetClue(y + 1, x + 1) == 3) {
				move m(DIFFICULTY_DIAGONAL_3);
				m.add(y * 2 + 0, x * 2 + 1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 1, x * 2 + 0, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 3, x * 2 + 4, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 4, x * 2 + 3, SlitherlinkField::LOOP_LINE);

				moves.push_back(m);
			}
			if (y != field.GetHeight() - 1 && x != 0 && field.GetClue(y + 1, x - 1) == 3) {
				move m(DIFFICULTY_DIAGONAL_3);
				m.add(y * 2 + 1, x * 2 + 2, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 0, x * 2 + 1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 3, x * 2 - 2, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 4, x * 2 - 1, SlitherlinkField::LOOP_LINE);

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

		if (style == SlitherlinkField::LOOP_LINE) ++n_lines;
		else if (style == SlitherlinkField::LOOP_BLANK) ++n_blanks;
	}

	if (n_lines == clue) {
		for (int i = 0; i < 4; ++i) {
			int y2 = y * 2 + 1 + GridConstant::GRID_DY[i], x2 = x * 2 + 1 + GridConstant::GRID_DX[i];

			if (field.GetSegmentStyle(y2, x2) == SlitherlinkField::LOOP_UNDECIDED) {
				moves.push_back(move(y2, x2, SlitherlinkField::LOOP_BLANK, DIFFICULTY_ADJACENT_LINES[clue]));
			}
		}
	}
	if (4 - n_blanks == clue) {
		for (int i = 0; i < 4; ++i) {
			int y2 = y * 2 + 1 + GridConstant::GRID_DY[i], x2 = x * 2 + 1 + GridConstant::GRID_DX[i];

			if (field.GetSegmentStyle(y2, x2) == SlitherlinkField::LOOP_UNDECIDED) {
				moves.push_back(move(y2, x2, SlitherlinkField::LOOP_LINE, DIFFICULTY_ADJACENT_LINES[clue]));
			}
		}
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
		
		if (field.GetSegmentStyleSafe(y * 2 + 1 + dy1 * 2 + dy2, x * 2 + 1 + dx1 * 2 + dx2) == SlitherlinkField::LOOP_BLANK &&
			field.GetSegmentStyleSafe(y * 2 + 1 + dy2 * 2 + dy1, x * 2 + 1 + dx2 * 2 + dx1) == SlitherlinkField::LOOP_BLANK) {
			if (clue == 1) {
				move m(DIFFICULTY_CORNER_CLUE[1]);
				m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, SlitherlinkField::LOOP_BLANK);
				m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, SlitherlinkField::LOOP_BLANK);
				moves.push_back(m);
			}
			if (clue == 3) {
				move m(DIFFICULTY_CORNER_CLUE[3]);
				m.add(y * 2 + 1 + dy1, x * 2 + 1 + dx1, SlitherlinkField::LOOP_LINE);
				m.add(y * 2 + 1 + dy2, x * 2 + 1 + dx2, SlitherlinkField::LOOP_LINE);
				moves.push_back(m);
			}
		}
	}
}

}