#include "KakuroField.h"
#include "KakuroProblem.h"
#include "../common/MiniVector.hpp"
#include <cstdio>

namespace Penciloid
{
KakuroField::KakuroField() : cells(nullptr), height(0), width(0), field_status(SolverStatus::NORMAL), num_undecided_cells(0)
{
}

KakuroField::~KakuroField()
{
	if (cells) delete[] cells;
}

void KakuroField::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (cells) delete[] cells;

	cells = new KakuroCell[height * width * 2];
	field_status = SolverStatus::NORMAL;
	num_undecided_cells = 0;
}

void KakuroField::Init(KakuroProblem &prob)
{
	Init(prob.GetHeight(), prob.GetWidth());

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (prob.IsNumberCell(y, x)) SetNumberCell(y, x);

			if (prob.IsHintAvailableVertical(y, x)) {
				int y_end = y + 1;
				for (; y_end < height && prob.IsNumberCell(y_end, x); ++y_end);

				for (int y2 = y + 1; y2 < y_end; ++y2) {
					cells[CellId(y2, x)].group_num_cells = y_end - (y + 1);
					cells[CellId(y2, x)].group_sum = prob.GetHintVertical(y, x);
					if (y2 == y_end - 1) cells[CellId(y2, x)].group_next_cell = CellId(y + 1, x);
					else cells[CellId(y2, x)].group_next_cell = CellId(y2 + 1, x);
				}
			}

			if (prob.IsHintAvailableHorizontal(y, x)) {
				int x_end = x + 1;
				for (; x_end < width && prob.IsNumberCell(y, x_end); ++x_end);

				for (int x2 = x + 1; x2 < x_end; ++x2) {
					cells[CellId(y, x2) | 1].group_num_cells = x_end - (x + 1);
					cells[CellId(y, x2) | 1].group_sum = prob.GetHintHorizontal(y, x);
					if (x2 == x_end - 1) cells[CellId(y, x2) | 1].group_next_cell = CellId(y, x + 1) | 1;
					else cells[CellId(y, x2) | 1].group_next_cell = CellId(y, x2 + 1) | 1;
				}
			}
		}
	}
}

int KakuroField::CheckAll()
{
	for (int i = 0; i < height * width * 2; ++i) {
		if (cells[i].group_next_cell != -1) CheckGroup(i);
	}

	return GetStatus();
}

void KakuroField::SetNumberCell(int y, int x)
{
	int id = CellId(y, x);

	if (cells[id].cell_value == CELL_HINT) {
		++num_undecided_cells;
		cells[id].cell_value = cells[id | 1].cell_value = CELL_UNDECIDED;
		cells[id].cell_candidate = cells[id | 1].cell_candidate = (2 << CELL_MAX_VALUE) - 2;
	}
}

int KakuroField::CheckGroup(int id)
{
	if (Next(id) == -1) return GetStatus();

	MiniVector<int, CELL_MAX_VALUE> group_cells;

	for (int current = id;;) {
		group_cells.push_back(current);
		if ((current = Next(current)) == id) break;
	}

	// TODO: desperately naive algorithm
	int already_used_values = 0;
	
	for (int current : group_cells) {
		if (cells[current].cell_value >= 1) already_used_values |= 1 << cells[current].cell_value;
	}

	int reserved_candidate = 0;
	for (int p : group_cells) {
		int cand = cells[p].cell_candidate;
		if (!IsUniqueCandidate(cand) && IsUniqueCandidate(cand ^ (cand & -cand))) {
			for (int q : group_cells) {
				if (p != q && cand == cells[q].cell_candidate) {
					reserved_candidate |= cand;
					for (int r : group_cells) {
						if (p != r && q != r) {
							UpdateCandidate(r, ~cand);
						}
					}
				}
			}
		}
	}

	int possible_candidate = 0, imperative_candidate = (2 << CELL_MAX_VALUE) - 2;
	
	for (int bits = 2; bits < (2 << CELL_MAX_VALUE); bits += 2) {
		int sum = 0, num = 0;

		for (int i = 1; i <= CELL_MAX_VALUE; ++i) {
			if (bits & (1 << i)) {
				sum += i;
				++num;
			}
		}

		if ((bits & reserved_candidate) != reserved_candidate || (bits & already_used_values) != already_used_values || sum != cells[id].group_sum || num != cells[id].group_num_cells) continue;

		for (int current : group_cells) {
			if ((cells[current].cell_candidate & bits) == 0) goto nex;
		}

		possible_candidate |= bits ^ already_used_values;
		imperative_candidate &= bits ^ already_used_values;

	nex:
		continue;
	}

	for (int current : group_cells) {
		if (cells[current].cell_value == CELL_UNDECIDED) UpdateCandidate(current, possible_candidate);
	}

	for (int i = 1; i <= CELL_MAX_VALUE; ++i) if (imperative_candidate & (1 << i)) {
		int candidate_loc = -1;

		for (int current : group_cells) {
			if (cells[current].cell_candidate & (1 << i)) {
				if (candidate_loc == -1) candidate_loc = current;
				else {
					candidate_loc = -2;
					break;
				}
			}
		}

		if (candidate_loc == -1) return UpdateStatus(SolverStatus::INCONSISTENT);
		else if (candidate_loc != -2) UpdateCandidate(candidate_loc, 1 << i);
	}

	return GetStatus();
}

int KakuroField::UpdateCandidate(int id, int new_candidate)
{
	id &= ~1;
	new_candidate &= cells[id].cell_candidate;

	if (new_candidate == 0) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} if (new_candidate == cells[id].cell_candidate) return UpdateStatus(0);

	cells[id].cell_candidate = cells[id | 1].cell_candidate = new_candidate;

	if (IsUniqueCandidate(new_candidate)) {
		int cand_value = 0;
		for (int i = 1; i <= CELL_MAX_VALUE; ++i) {
			if (new_candidate == 1 << i) {
				cand_value = i;
				break;
			}
		}

		cells[id].cell_value = cells[id | 1].cell_value = cand_value;
		if (--num_undecided_cells == 0) UpdateStatus(SolverStatus::SUCCESS);

		// TODO: purge determined cells?
	}

	CheckGroup(id);
	CheckGroup(id | 1);

	return GetStatus();
}

void KakuroField::Debug()
{
	for (int i_ = 0; i_ <= 2 * height; ++i_) {
		if (i_ % 2 == 0) {
			fprintf(stderr, "+");
			for (int j = 0; j < width; ++j) fprintf(stderr, "---+");
			fprintf(stderr, "\n");
			continue;
		}

		int i = i_ / 2;
		fprintf(stderr, "|");
		for (int j = 0; j < width; ++j) {
			if (IsNumberCell(i, j)) fprintf(stderr, "   |");
			else {
				if (j == width - 1 || !IsNumberCell(i, j + 1)) fprintf(stderr, "\\  |");
				else fprintf(stderr, "\\%2d|", cells[CellId(i, j + 1) | 1].group_sum);
			}
		}
		fprintf(stderr, "\n");

		fprintf(stderr, "|");
		for (int j = 0; j < width; ++j) {
			if (IsNumberCell(i, j)) {
				if (GetCellValue(i, j) == CELL_UNDECIDED) fprintf(stderr, "   |");
				else fprintf(stderr, " %d |", GetCellValue(i, j));
			} else {
				fprintf(stderr, " \\ |");
			}
		}
		fprintf(stderr, "\n");

		fprintf(stderr, "|");
		for (int j = 0; j < width; ++j) {
			if (IsNumberCell(i, j)) fprintf(stderr, "   |");
			else {
				if (i == height - 1 || !IsNumberCell(i + 1, j)) fprintf(stderr, "  \\|");
				else fprintf(stderr, "%2d\\|", cells[CellId(i + 1, j)].group_sum);
			}
		}
		fprintf(stderr, "\n");
	}
	fflush(stderr);
}

}

