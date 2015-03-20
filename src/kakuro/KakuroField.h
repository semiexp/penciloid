#pragma once

#include "../common/SolverConstant.h"

namespace Penciloid
{
class KakuroProblem;

class KakuroField
{
public:
	KakuroField();
	~KakuroField();

	void Init(int height_t, int width_t);
	void Init(KakuroProblem &prob);

	inline int GetStatus() const { return field_status; }
	int CheckAll();

	void SetNumberCell(int y, int x);
	bool IsNumberCell(int y, int x) { return cells[CellId(y, x)].cell_value != CELL_HINT; }
	// TODO: Add group modifier

	inline int GetCellValue(int y, int x) const { return cells[CellId(y, x)].cell_value; }

	void Debug();

	static const int CELL_HINT = -1;
	static const int CELL_UNDECIDED = 0;
	static const int CELL_MAX_VALUE = 9;

private:
	struct KakuroCell
	{
		int cell_value, cell_candidate;
		int group_next_cell, group_sum, group_num_cells;

		KakuroCell() : cell_value(CELL_HINT), cell_candidate(0), group_next_cell(-1), group_sum(0), group_num_cells(0) {}
	};

	KakuroCell *cells;
	int height, width;
	int num_undecided_cells;
	int field_status;

	int CellId(int y, int x) const { return (y * width + x) * 2; }
	inline int UpdateStatus(int status) { return field_status |= status; }
	inline int IsUniqueCandidate(int candidate) { return (candidate & -candidate) == candidate; }
	inline int Next(int id) { return cells[id].group_next_cell; }

	int CheckGroup(int id);
	int UpdateCandidate(int id, int new_candidate);
};

}
