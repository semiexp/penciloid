#include "SlitherlinkField.h"
#include "SlitherlinkProblem.h"
#include "SlitherlinkDatabase.h"

namespace Penciloid
{
SlitherlinkField::SlitherlinkField()
{
	hints = nullptr;
	assumption_table = nullptr;
}

SlitherlinkField::~SlitherlinkField()
{
	if (hints) delete[] hints;
	if (assumption_table) delete[] assumption_table;
}

void SlitherlinkField::Init(int height_t, int width_t, bool enable_assumption_table)
{
	if (assumption_table) delete[] assumption_table;
	assumption_table = nullptr;

	GridLoop<SlitherlinkField>::Init(height_t, width_t);

	if (enable_assumption_table) {
		assumption_table = new bool[(2 * GetHeight() + 1) * (2 * GetWidth() + 1)];
		std::fill(assumption_table, assumption_table + (2 * GetHeight() + 1) + (2 * GetWidth() + 1), false);
	} else assumption_table = nullptr;

	if (hints) delete[] hints;
	hints = new int[GetHeight() * GetWidth()];

	for (int i = 0; i < GetHeight() * GetWidth(); ++i) hints[i] = HINT_NONE;
}

void SlitherlinkField::Init(SlitherlinkField &field)
{
	if (assumption_table) delete[] assumption_table;
	assumption_table = nullptr;

	GridLoop<SlitherlinkField>::Init(field);

	if (hints) delete[] hints;
	hints = new int[GetHeight() * GetWidth()];
	memcpy(hints, field.hints, sizeof(int) * GetHeight() * GetWidth());

	if (field.assumption_table != nullptr) {
		assumption_table = new bool[(2 * GetHeight() + 1) * (2 * GetWidth() + 1)];
		memcpy(assumption_table, field.assumption_table, sizeof(bool) * (2 * GetHeight() + 1) + (2 * GetWidth() + 1));
	} else assumption_table = nullptr;
}

void SlitherlinkField::Init(SlitherlinkProblem &prob, bool enable_assumption_table)
{
	if (assumption_table) delete[] assumption_table;
	assumption_table = nullptr;

	Init(prob.GetHeight(), prob.GetWidth());

	if (enable_assumption_table) {
		assumption_table = new bool[(2 * GetHeight() + 1) * (2 * GetWidth() + 1)];
		std::fill(assumption_table, assumption_table + (2 * GetHeight() + 1) + (2 * GetWidth() + 1), false);
	} else assumption_table = nullptr;

	for (int i = 0; i < GetHeight(); ++i) {
		for (int j = 0; j < GetWidth(); ++j) {
			int hint = prob.GetHint(i, j);

			if (hint != HINT_NONE) SetHint(i, j, hint);
		}
	}
}

int SlitherlinkField::SetHint(int y, int x, int hint)
{
	int id = CellId(y, x);

	if (hints[id] != HINT_NONE) {
		if (hints[id] != hint) {
			return UpdateStatus(SolverStatus::INCONSISTENT);
		}
		return UpdateStatus(0);
	}

	if (GetStatus() & SolverStatus::SUCCESS) {
		int adjacent_lines = 0;
		for (int i = 0; i < 4; ++i) {
			if (GetSegmentStyle(y * 2 + 1 + GridConstant::GRID_DY[i], x * 2 + 1 + GridConstant::GRID_DX[i]) == LOOP_LINE) ++adjacent_lines;
		}

		if (adjacent_lines != hint) {
			return UpdateStatus(SolverStatus::INCONSISTENT);
		}
	}

	if (assumption_table != nullptr) {
		int y2 = y * 2 + 1, x2 = x * 2 + 1;

		for (int i = 0; i < 12; ++i) {
			SetAssumptionCandidate(y2 + SlitherlinkDatabase::DATABASE_DY[i], x2 + SlitherlinkDatabase::DATABASE_DX[i]);
		}
	}

	hints[id] = hint;
	CheckTheorem(y * 2 + 1, x * 2 + 1);
	CheckCellSpecific(y * 2 + 1, x * 2 + 1);

	return GetStatus();
}

void SlitherlinkField::SegmentDetermined(int y, int x)
{
	if (assumption_table != nullptr) {
		if (y % 2 == 1 && x % 2 == 0) {
			SetAssumptionCandidate(y - 2, x);
			SetAssumptionCandidate(y - 1, x - 1);
			SetAssumptionCandidate(y - 1, x + 1);
			SetAssumptionCandidate(y + 1, x - 1);
			SetAssumptionCandidate(y + 1, x + 1);
			SetAssumptionCandidate(y + 2, x);
		} else {
			SetAssumptionCandidate(y, x - 2);
			SetAssumptionCandidate(y - 1, x - 1);
			SetAssumptionCandidate(y + 1, x - 1);
			SetAssumptionCandidate(y - 1, x + 1);
			SetAssumptionCandidate(y + 1, x + 1);
			SetAssumptionCandidate(y, x + 2);
		}
	}
}

void SlitherlinkField::CheckTheorem(int y, int x)
{
	if (GetHint(y / 2, x / 2) == 3) {
		// vertically or horizontally adjacent 3

		for (int i = 0; i < 4; ++i) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];
			if (GetHintSafe(y / 2 + dy, x / 2 + dx) == 3) {
				DetermineLine(y - dy, x - dx);
				DetermineLine(y + dy, x + dx);
				DetermineBlank(y + dy + 2 * dx, x + dx + 2 * dy);
				DetermineBlank(y + dy - 2 * dx, x + dx - 2 * dy);
				DetermineLine(y + 3 * dy, x + 3 * dx);
			}

			int dy2 = GridConstant::GRID_DY[(i + 1) % 4], dx2 = GridConstant::GRID_DX[(i + 1) % 4];
			if (GetHintSafe(y / 2 + dy + dy2, x / 2 + dx + dx2) == 3) {
				DetermineLine(y - dy, x - dx);
				DetermineLine(y - dy2, x - dx2);
				DetermineLine(y + dy * 3 + dy2 * 2, x + dx * 3 + dx2 * 2);
				DetermineLine(y + dy2 * 3 + dy * 2, x + dx2 * 3 + dx * 2);
			}
		}
	}
}

void SlitherlinkField::CheckCellSpecific(int y, int x)
{
	int id = CellId(y / 2, x / 2);

	if (hints == nullptr || hints[id] == HINT_NONE) return;

	if (method.diagonal_chain) CheckDiagonalChain(y, x);

	if (SlitherlinkDatabase::IsCreated()) {
		int db_index = hints[id];

		for (int i = 11; i >= 0; --i) {
			db_index = db_index * 3 + GetSegmentStyleSafe(y + SlitherlinkDatabase::DATABASE_DY[i], x + SlitherlinkDatabase::DATABASE_DX[i]);
		}

		int new_status = SlitherlinkDatabase::database[db_index];

		if (new_status == -1) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		}

		for (int i = 0; i < 12; ++i) {
			if ((new_status & 3) == 1) {
				DetermineLine(y + SlitherlinkDatabase::DATABASE_DY[i], x + SlitherlinkDatabase::DATABASE_DX[i]);
			} else if ((new_status & 3) == 2) {
				DetermineBlank(y + SlitherlinkDatabase::DATABASE_DY[i], x + SlitherlinkDatabase::DATABASE_DX[i]);
			}

			new_status >>= 2;
		}

		return;
	}

	static const int bits[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3};

	int segment_line = 15, segment_blank = 15;

	for (int i = 0; i < 15; ++i) {
		if (hints[id] != bits[i]) continue;

		// checking the consistency with the current status

		for (int j = 0; j < 4; ++j) {
			int style = GetSegmentStyle(y + GridConstant::GRID_DY[j], x + GridConstant::GRID_DX[j]);
			if ((style == LOOP_LINE && ((i & (1 << j)) == 0)) || (style == LOOP_BLANK && ((i & (1 << j)) != 0))) goto fail;
		}

		for (int j = 0; j < 4; ++j) {
			int dy1 = GridConstant::GRID_DY[j], dx1 = GridConstant::GRID_DX[j];
			int dy2 = GridConstant::GRID_DY[(j + 1) & 3], dx2 = GridConstant::GRID_DX[(j + 1) & 3];

			int num_lines = 0, num_blanks = 0;

			if (i & (1 << j)) ++num_lines;
			else ++num_blanks;

			if (i & (1 << ((j + 1) & 3))) ++num_lines;
			else ++num_blanks;

			int style1 = GetSegmentStyleSafe(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2);

			if (style1 == LOOP_LINE) ++num_lines;
			else if (style1 == LOOP_BLANK) ++num_blanks;

			int style2 = GetSegmentStyleSafe(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2);

			if (style2 == LOOP_LINE) ++num_lines;
			else if (style2 == LOOP_BLANK) ++num_blanks;

			if (num_lines >= 3 || (num_lines == 1 && num_blanks == 3)) goto fail;
		}

		segment_line &= i;
		segment_blank &= ~i;

	fail:
		continue;
	}

	if (segment_line & segment_blank) {
		UpdateStatus(SolverStatus::INCONSISTENT);
		return;
	}

	for (int i = 0; i < 4; ++i) {
		if (segment_line & (1 << i)) {
			DetermineLine(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
		}

		if (segment_blank & (1 << i)) {
			DetermineBlank(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
		}
	}

	if (hints[id] == 3) {
		for (int i = 0; i < 4; ++i) {
			int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
			int dy2 = GridConstant::GRID_DY[(i + 1) & 3], dx2 = GridConstant::GRID_DX[(i + 1) & 3];

			if (GetSegmentStyleSafe(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2) == LOOP_LINE) {
				DetermineBlank(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2);
			}
			if (GetSegmentStyleSafe(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2) == LOOP_LINE) {
				DetermineBlank(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2);
			}
		}
	}
}

void SlitherlinkField::CheckDiagonalChain(int y, int x)
{
	for (int dir = 0; dir < 4; ++dir){
		int dy1 = GridConstant::GRID_DY[dir], dx1 = GridConstant::GRID_DX[dir];
		int dy2 = GridConstant::GRID_DY[(dir + 1) & 3], dx2 = GridConstant::GRID_DX[(dir + 1) & 3];

		int line_in = -1;

		int style1 = GetSegmentStyle(y + dy1, x + dx1);
		int style2 = GetSegmentStyle(y + dy2, x + dx2);

		if (style1 != LOOP_UNDECIDED && style2 != LOOP_UNDECIDED) {
			line_in = (style1 == LOOP_LINE ? 1 : 0) + (style2 == LOOP_LINE ? 1 : 0);
		}

		int style3 = GetSegmentStyle(y - dy1, x - dx1);
		int style4 = GetSegmentStyle(y - dy2, x - dx2);

		if (GetHint(y / 2, x / 2) != HINT_NONE && style3 != LOOP_UNDECIDED && style4 != LOOP_UNDECIDED) {
			line_in = GetHint(y / 2, x / 2) - ((style3 == LOOP_LINE ? 1 : 0) + (style4 == LOOP_LINE ? 1 : 0));
		}

		if (line_in != -1) {
			line_in &= 1;

			int current_y = y + 2 * (dy1 + dy2), current_x = x + 2 * (dx1 + dx2);
			while (true) {
				if (current_y < 0 || current_x < 0 || current_y > 2 * GetHeight() || current_x > 2 * GetWidth()) break;
				
				int hint2 = GetHint(current_y / 2, current_x / 2);

				if (hint2 == HINT_NONE || hint2 == 0) break;
				if (hint2 == 2) {
					current_y += 2 * (dy1 + dy2);
					current_x += 2 * (dx1 + dx2);
					continue;
				}
				if (hint2 == 1) {
					if (line_in == 0) {
						DetermineBlank(current_y - dy1, current_x - dx1);
						DetermineBlank(current_y - dy2, current_x - dx2);
					} else if (line_in == 1) {
						DetermineBlank(current_y + dy1, current_x + dx1);
						DetermineBlank(current_y + dy2, current_x + dx2);
					}
					break;
				}
				if (hint2 == 3) {
					if (line_in == 0) {
						DetermineLine(current_y - dy1, current_x - dx1);
						DetermineLine(current_y - dy2, current_x - dx2);
					} else if (line_in == 1) {
						DetermineLine(current_y + dy1, current_x + dx1);
						DetermineLine(current_y + dy2, current_x + dx2);
					}
					break;
				}
			}
		}
	}
}

int SlitherlinkField::Assume()
{
	SlitherlinkField tmp_line, tmp_blank;
	bool is_updated;

	do {
		is_updated = false;
		CheckAll();

		for (int i = 0; i <= GetHeight() * 2; ++i) {
			for (int j = 0; j <= GetWidth() * 2; ++j) {
				if ((i & 1) == (j & 1)) continue;

				if (assumption_table) {
					if (!assumption_table[i * (GetWidth() * 2 + 1) + j]) continue;
				}

				if (i & 1) {
					if (GetSegmentStyleSafe(i - 1, j - 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i - 1, j + 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i + 1, j - 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i + 1, j + 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i - 2, j) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i + 2, j) == LOOP_UNDECIDED)
						continue;
				} else {
					if (GetSegmentStyleSafe(i - 1, j - 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i - 1, j + 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i + 1, j - 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i + 1, j + 1) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i, j - 2) == LOOP_UNDECIDED
						&& GetSegmentStyleSafe(i, j + 2) == LOOP_UNDECIDED)
						continue;
				}

				if (GetSegmentStyle(i, j) == LOOP_UNDECIDED && IsRepresentative(i, j)) {
					if (assumption_table) assumption_table[i * (GetWidth() * 2 + 1) + j] = false;

					tmp_line.Init(*this);
					tmp_blank.Init(*this);

					tmp_line.DetermineLine(i, j);
					tmp_blank.DetermineBlank(i, j);

					if ((tmp_line.GetStatus() & SolverStatus::INCONSISTENT) && (tmp_blank.GetStatus() & SolverStatus::INCONSISTENT)) {
						return UpdateStatus(SolverStatus::INCONSISTENT);
					} else if (tmp_line.GetStatus() & SolverStatus::INCONSISTENT) {
						DetermineBlank(i, j);
						is_updated = true;
					} else if (tmp_blank.GetStatus() & SolverStatus::INCONSISTENT) {
						DetermineLine(i, j);
						is_updated = true;
					}
				}
			}
		}
	} while (is_updated);
	return GetStatus();
}

void SlitherlinkField::Debug()
{
	for (int i = 0; i <= GetHeight() * 2; ++i) {
		for (int j = 0; j <= GetWidth() * 2; ++j) {
			if (i % 2 == 0 && j % 2 == 0) {
				fprintf(stderr, "+");
			} else if (i % 2 == 0 && j % 2 == 1) {
				int style = GetSegmentStyle(i, j);
				if (style == LOOP_UNDECIDED) fprintf(stderr, "   ");
				if (style == LOOP_LINE) fprintf(stderr, "---");
				if (style == LOOP_BLANK) fprintf(stderr, " X ");
			} else if (i % 2 == 1 && j % 2 == 0) {
				int style = GetSegmentStyle(i, j);
				if (style == LOOP_UNDECIDED) fprintf(stderr, " ");
				if (style == LOOP_LINE) fprintf(stderr, "|");
				if (style == LOOP_BLANK) fprintf(stderr, "X");
			} else if (i % 2 == 1 && j % 2 == 1) {
				if (GetHint(i / 2, j / 2) == HINT_NONE) fprintf(stderr, "   ");
				else fprintf(stderr, " %d ", GetHint(i / 2, j / 2));
			}
		}
		fprintf(stderr, "\n");
	}
}

}