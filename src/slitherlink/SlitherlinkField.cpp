#include "SlitherlinkField.h"
#include "SlitherlinkProblem.h"

namespace Penciloid
{
SlitherlinkField::SlitherlinkField()
{
	height = 0;
	width = 0;

	hints = nullptr;
}

SlitherlinkField::~SlitherlinkField()
{
	if (hints) delete[] hints;
}

void SlitherlinkField::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (hints) delete[] hints;
	hints = new int[height * width];

	for (int i = 0; i < height * width; ++i) hints[i] = HINT_NONE;

	auxiliary.Init(this);
	grid.SetAuxiliarySolver(&auxiliary);
	grid.Init(height, width);
}

void SlitherlinkField::Init(SlitherlinkProblem &prob)
{
	Init(prob.GetHeight(), prob.GetWidth());

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
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
			return grid.UpdateStatus(SolverStatus::INCONSISTENT);
		}
		return grid.UpdateStatus(0);
	}

	hints[id] = hint;
	CheckTheorem(y * 2 + 1, x * 2 + 1);
	CheckCell(y * 2 + 1, x * 2 + 1);

	return grid.GetStatus();
}

void SlitherlinkField::CheckTheorem(int y, int x)
{
	if (GetHint(y / 2, x / 2) == 3) {
		// vertically or horizontally adjacent 3

		for (int i = 0; i < 4; ++i) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];
			if (GetHintSafe(y / 2 + dy, x / 2 + dx) == 3) {
				grid.DetermineLine(y - dy, x - dx);
				grid.DetermineLine(y + dy, x + dx);
				grid.DetermineBlank(y + dy + 2 * dx, x + dx + 2 * dy);
				grid.DetermineBlank(y + dy - 2 * dx, x + dx - 2 * dy);
				grid.DetermineLine(y + 3 * dy, x + 3 * dx);
			}

			int dy2 = GridConstant::GRID_DY[(i + 1) % 4], dx2 = GridConstant::GRID_DX[(i + 1) % 4];
			if (GetHintSafe(y / 2 + dy + dy2, x / 2 + dx + dx2) == 3) {
				grid.DetermineLine(y - dy, x - dx);
				grid.DetermineLine(y - dy2, x - dx2);
				grid.DetermineLine(y + dy * 3 + dy2 * 2, x + dx * 3 + dx2 * 2);
				grid.DetermineLine(y + dy2 * 3 + dy * 2, x + dx2 * 3 + dx * 2);
			}
		}
	}
}

void SlitherlinkField::CheckCell(int y, int x)
{
	int id = CellId(y / 2, x / 2);

	if (hints[id] == HINT_NONE) return;

	// TODO: faster checker is required

	static const int bits[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3};

	int segment_line = 15, segment_blank = 15;

	for (int i = 0; i < 15; ++i) {
		if (hints[id] != bits[i]) continue;

		// checking the consistency with the current status

		for (int j = 0; j < 4; ++j) {
			int style = grid.GetSegmentStyle(y + GridConstant::GRID_DY[j], x + GridConstant::GRID_DX[j]);
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

			int style1 = grid.GetSegmentStyleSafe(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2);

			if (style1 == LOOP_LINE) ++num_lines;
			else if (style1 == LOOP_BLANK) ++num_blanks;

			int style2 = grid.GetSegmentStyleSafe(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2);

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
		grid.UpdateStatus(SolverStatus::INCONSISTENT);
		return;
	}

	for (int i = 0; i < 4; ++i) {
		if (segment_line & (1 << i)) {
			grid.DetermineLine(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
		}

		if (segment_blank & (1 << i)) {
			grid.DetermineBlank(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
		}
	}

	if (hints[id] == 3) {
		for (int i = 0; i < 4; ++i) {
			int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
			int dy2 = GridConstant::GRID_DY[(i + 1) & 3], dx2 = GridConstant::GRID_DX[(i + 1) & 3];

			if (grid.GetSegmentStyleSafe(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2) == LOOP_LINE) {
				grid.DetermineBlank(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2);
			}
			if (grid.GetSegmentStyleSafe(y + dy2 + dy1 * 2, x + dx2 + dx1 * 2) == LOOP_LINE) {
				grid.DetermineBlank(y + dy1 + dy2 * 2, x + dx1 + dx2 * 2);
			}
		}
	}
}

void SlitherlinkField::Debug()
{
	for (int i = 0; i <= height * 2; ++i) {
		for (int j = 0; j <= width * 2; ++j) {
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