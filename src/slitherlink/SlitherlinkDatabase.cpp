#include "SlitherlinkDatabase.h"
#include "SlitherlinkDatabaseMethod.hpp"
#include "../common/SolverConstant.h"

namespace Penciloid
{
int *SlitherlinkDatabase::database = nullptr;
const int SlitherlinkDatabase::DATABASE_DX[] = { -2, -2, -1, -1, -1, 0, 0, 1, 1, 1, 2, 2 };
const int SlitherlinkDatabase::DATABASE_DY[] = { -1, 1, -2, 0, 2, -1, 1, -2, 0, 2, -1, 1 };

void SlitherlinkDatabase::CreateDatabase()
{
	const int pow3[] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441 };

	ReleaseDatabase();
	database = new int[pow3[12] * 4];

	int styles[12];

	for (int hint = 0; hint <= 3; ++hint) {
		for (int id = pow3[12] - 1; id >= 0; --id) {
			int tmp = id;
			int undecided_pos = -1;

			for (int i = 0; i < 12; ++i) {
				styles[i] = tmp % 3;
				tmp /= 3;

				if (styles[i] == 0) undecided_pos = i;
			}

			if (undecided_pos == -1) {
				bool consistency = true;

				if (8 - (styles[3] + styles[5] + styles[6] + styles[8]) != hint) {
					consistency = false;
				}

				tmp = 8 - (styles[0] + styles[2] + styles[3] + styles[5]);
				if (tmp != 0 && tmp != 2) consistency = false;
				tmp = 8 - (styles[1] + styles[3] + styles[4] + styles[6]);
				if (tmp != 0 && tmp != 2) consistency = false;
				tmp = 8 - (styles[5] + styles[7] + styles[8] + styles[10]);
				if (tmp != 0 && tmp != 2) consistency = false;
				tmp = 8 - (styles[6] + styles[8] + styles[9] + styles[11]);
				if (tmp != 0 && tmp != 2) consistency = false;

				if (consistency) {
					int tmp = 0;
					for (int i = 0; i < 12; ++i) tmp |= styles[i] << (2 * i);
					database[hint * pow3[12] + id] = tmp;
				} else {
					database[hint * pow3[12] + id] = -1;
				}
			} else {
				int base1 = database[hint * pow3[12] + id + pow3[undecided_pos]];
				int base2 = database[hint * pow3[12] + id + pow3[undecided_pos] * 2];

				database[hint * pow3[12] + id] = base1 & base2;
			}
		}
	}
}

void SlitherlinkDatabase::CreateReducedDatabase(SlitherlinkDatabaseMethod &method)
{
	const int pow3[] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441 };

	ReleaseDatabase();
	database = new int[pow3[12] * 4];

	int styles[12];

	for (int hint = 0; hint <= 3; ++hint) {
		for (int id = pow3[12] - 1; id >= 0; --id) {
			int tmp = id;

			for (int i = 0; i < 12; ++i) {
				styles[i] = tmp % 3;
				tmp /= 3;
			}

			database[hint * pow3[12] + id] = SolveLocal(hint, styles, method);
		}
	}
}

void SlitherlinkDatabase::ReleaseDatabase()
{
	if (database) {
		delete[] database;
		database = nullptr;
	}
}

int SlitherlinkDatabase::SolveLocal(int clue, int styles[12], SlitherlinkDatabaseMethod &method)
{
	const int UNDECIDED = 0, LINE = 1, BLANK = 2;
	int segments[5][5];
	int ret = 0, ret_previous = 0;

	for (int i = 0; i < 12; ++i) segments[2 + DATABASE_DY[i]][2 + DATABASE_DX[i]] = styles[i];

	int nt = 0;
	do {
		ret_previous = ret;

		if (method.vertex_rule) {
			for (int i = 0; i < 4; ++i) {
				int dys[] = { i / 2 * 2 + 0, i / 2 * 2 + 1, i / 2 * 2 + 1, i / 2 * 2 + 2 };
				int dxs[] = { i % 2 * 2 + 1, i % 2 * 2 + 0, i % 2 * 2 + 2, i % 2 * 2 + 1 };

				int n_lines = 0, n_blanks = 0;
				for (int j = 0; j < 4; ++j) {
					if (segments[dys[j]][dxs[j]] == LINE) ++n_lines;
					if (segments[dys[j]][dxs[j]] == BLANK) ++n_blanks;
				}

				if (n_lines >= 3 || (n_lines == 1 && n_blanks == 3)) return -1;
				if (n_lines == 2 || n_blanks >= 3) {
					for (int j = 0; j < 4; ++j) {
						if (segments[dys[j]][dxs[j]] == UNDECIDED) segments[dys[j]][dxs[j]] = BLANK;
					}
				}
				if (n_lines == 1 && n_blanks == 2) {
					for (int j = 0; j < 4; ++j) {
						if (segments[dys[j]][dxs[j]] == UNDECIDED) segments[dys[j]][dxs[j]] = LINE;
					}
				}
			}
		}

		if (method.adjacent_lines) {
			int n_lines =
				(segments[2][1] == LINE ? 1 : 0) +
				(segments[2][3] == LINE ? 1 : 0) +
				(segments[1][2] == LINE ? 1 : 0) +
				(segments[3][2] == LINE ? 1 : 0);
			int n_blanks =
				(segments[2][1] == BLANK ? 1 : 0) +
				(segments[2][3] == BLANK ? 1 : 0) +
				(segments[1][2] == BLANK ? 1 : 0) +
				(segments[3][2] == BLANK ? 1 : 0);

			if (n_lines > clue || 4 - n_blanks < clue) return -1;

			if (n_lines == clue) {
				if (segments[2][1] == UNDECIDED) segments[2][1] = BLANK;
				if (segments[2][3] == UNDECIDED) segments[2][3] = BLANK;
				if (segments[1][2] == UNDECIDED) segments[1][2] = BLANK;
				if (segments[3][2] == UNDECIDED) segments[3][2] = BLANK;
			}
			if (4 - n_blanks == clue) {
				if (segments[2][1] == UNDECIDED) segments[2][1] = LINE;
				if (segments[2][3] == UNDECIDED) segments[2][3] = LINE;
				if (segments[1][2] == UNDECIDED) segments[1][2] = LINE;
				if (segments[3][2] == UNDECIDED) segments[3][2] = LINE;
			}
		}

		for (int dir = 0; dir < 4; ++dir) {
			int dy1 = GridConstant::GRID_DY[dir], dx1 = GridConstant::GRID_DX[dir];
			int dy2 = GridConstant::GRID_DY[(dir + 1) % 4], dx2 = GridConstant::GRID_DX[(dir + 1) % 4];

			if (segments[2 + dy1 * 2 + dy2][2 + dx1 * 2 + dx2] == BLANK && segments[2 + dy1 + dy2 * 2][2 + dx1 + dx2 * 2] == BLANK) {
				// this cell is corner

				if (clue == 1 && method.corner_1) {
					segments[2 + dy1][2 + dx1] = segments[2 + dy2][2 + dx2] = BLANK;
				}
				if (clue == 2 && method.corner_2) {
					if (segments[2 + dy1][2 + dx1] == LINE || segments[2 + dy2][2 + dx2] == LINE ||
						segments[2 - dy1][2 - dx1] == BLANK || segments[2 - dy2][2 - dx2] == BLANK) {
						segments[2 + dy1][2 + dx1] |= LINE;
						segments[2 + dy2][2 + dx2] |= LINE;
						segments[2 - dy1][2 - dx1] |= BLANK;
						segments[2 - dy2][2 - dx2] |= BLANK;
					}

					if (segments[2 + dy1][2 + dx1] == BLANK || segments[2 + dy2][2 + dx2] == BLANK ||
						segments[2 - dy1][2 - dx1] == LINE || segments[2 - dy2][2 - dx2] == LINE) {
						segments[2 + dy1][2 + dx1] |= BLANK;
						segments[2 + dy2][2 + dx2] |= BLANK;
						segments[2 - dy1][2 - dx1] |= LINE;
						segments[2 - dy2][2 - dx2] |= LINE;
					}

					int out_y1, out_x1, out_y2, out_x2;

					out_y1 = 2 + dy1 * 2 - dy2, out_x1 = 2 + dx1 * 2 - dx2;
					out_y2 = 2 + dy1 - dy2 * 2, out_x2 = 2 + dx1 - dx2 * 2;

					if (segments[out_y1][out_x1] == LINE) segments[out_y2][out_x2] |= BLANK;
					if (segments[out_y1][out_x1] == BLANK) segments[out_y2][out_x2] |= LINE;
					if (segments[out_y2][out_x2] == LINE) segments[out_y1][out_x1] |= BLANK;
					if (segments[out_y2][out_x2] == BLANK) segments[out_y1][out_x1] |= LINE;

					out_y1 = 2 - dy1 * 2 + dy2, out_x1 = 2 - dx1 * 2 + dx2;
					out_y2 = 2 - dy1 + dy2 * 2, out_x2 = 2 - dx1 + dx2 * 2;

					if (segments[out_y1][out_x1] == LINE) segments[out_y2][out_x2] |= BLANK;
					if (segments[out_y1][out_x1] == BLANK) segments[out_y2][out_x2] |= LINE;
					if (segments[out_y2][out_x2] == LINE) segments[out_y1][out_x1] |= BLANK;
					if (segments[out_y2][out_x2] == BLANK) segments[out_y1][out_x1] |= LINE;
				}
				if (clue == 3 && method.corner_3) {
					segments[2 + dy1][2 + dx1] = segments[2 + dy2][2 + dx2] = LINE;
				}
			}

			int in_y1, in_x1, in_y2, in_x2;
			in_y1 = 2 + dy1 * 2 + dy2, in_x1 = 2 + dx1 * 2 + dx2;
			in_y2 = 2 + dy1 + dy2 * 2, in_x2 = 2 + dx1 + dx2 * 2;

			if (clue == 3 && method.line_to_3) {
				if (segments[in_y1][in_x1] == LINE || segments[in_y2][in_x2] == LINE) {
					segments[2 - dy1][2 - dx1] |= LINE;
					segments[2 - dy2][2 - dx2] |= LINE;

					if (segments[in_y1][in_x1] == LINE) segments[in_y2][in_x2] |= BLANK;
					if (segments[in_y2][in_x2] == LINE) segments[in_y1][in_x1] |= BLANK;
				}
			}

			if (clue == 1 && method.line_to_1) {
				if ((segments[in_y1][in_x1] == LINE && segments[in_y2][in_x2] == BLANK) ||
					(segments[in_y1][in_x1] == BLANK && segments[in_y2][in_x2] == LINE)) {
					segments[2 - dy1][2 - dx1] |= BLANK;
					segments[2 - dy2][2 - dx2] |= BLANK;
				}
			}

			if (clue == 2 && method.line_to_2) {
				if ((segments[in_y1][in_x1] == LINE && segments[in_y2][in_x2] == BLANK) ||
					(segments[in_y1][in_x1] == BLANK && segments[in_y2][in_x2] == LINE)) {
					int out_y1, out_x1, out_y2, out_x2;

					out_y1 = 2 - dy1, out_x1 = 2 - dx1;
					out_y2 = 2 - dy2, out_x2 = 2 - dx2;

					if (segments[out_y1][out_x1] == BLANK) segments[out_y2][out_x2] |= LINE;
					if (segments[out_y1][out_x1] == LINE) segments[out_y2][out_x2] |= BLANK;
					if (segments[out_y2][out_x2] == BLANK) segments[out_y1][out_x1] |= LINE;
					if (segments[out_y2][out_x2] == LINE) segments[out_y1][out_x1] |= BLANK;

					out_y1 = 2 - dy1 * 2 - dy2, out_x1 = 2 - dx1 * 2 - dx2;
					out_y2 = 2 - dy1 - dy2 * 2, out_x2 = 2 - dx1 - dx2 * 2;

					if (segments[out_y1][out_x1] == BLANK) segments[out_y2][out_x2] |= LINE;
					if (segments[out_y1][out_x1] == LINE) segments[out_y2][out_x2] |= BLANK;
					if (segments[out_y2][out_x2] == BLANK) segments[out_y1][out_x1] |= LINE;
					if (segments[out_y2][out_x2] == LINE) segments[out_y1][out_x1] |= BLANK;
				}
			}

			if ((clue == 1 && method.line_from_1 && segments[2 - dy1][2 - dx1] == BLANK && segments[2 - dy2][2 - dx2] == BLANK) ||
				(clue == 3 && method.line_from_3 && segments[2 - dy1][2 - dx1] == LINE && segments[2 - dy2][2 - dx2] == LINE)) {
				if (segments[in_y1][in_x1] == LINE) segments[in_y2][in_x2] |= BLANK;
				if (segments[in_y1][in_x1] == BLANK) segments[in_y2][in_x2] |= LINE;
				if (segments[in_y2][in_x2] == LINE) segments[in_y1][in_x1] |= BLANK;
				if (segments[in_y2][in_x2] == BLANK) segments[in_y1][in_x1] |= LINE;
			}
		}

		ret = 0;
		for (int i = 0; i < 12; ++i) {
			int y = 2 + DATABASE_DY[i], x = 2 + DATABASE_DX[i];
			if (segments[y][x] == 3) return -1;
			ret |= segments[y][x] << (2 * i);
		}
	} while (ret != ret_previous);

	return ret;
}
}
