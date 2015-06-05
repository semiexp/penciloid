#include "SlitherlinkDatabase.h"

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

void SlitherlinkDatabase::CreateReducedDatabase()
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

			database[hint * pow3[12] + id] = SolveLocal(hint, styles);
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

int SlitherlinkDatabase::SolveLocal(int hint, int styles[12])
{
	int ret = 0;

	for (int i = 0; i < 12; ++i) ret |= styles[i] << (2 * i);

	// around the cell
#define AT(x) ((ret >> (x * 2)) & 3)
#define SET_L(x) (ret |= 1 << (x * 2))
#define SET_B(x) (ret |= 2 << (x * 2))

	int n_lines = (AT(3) == 1 ? 1 : 0) + (AT(5) == 1 ? 1 : 0) + (AT(6) == 1 ? 1 : 0) + (AT(8) == 1 ? 1 : 0);
	int n_blanks = (AT(3) == 2 ? 1 : 0) + (AT(5) == 2 ? 1 : 0) + (AT(6) == 2 ? 1 : 0) + (AT(8) == 2 ? 1 : 0);

	if (n_lines > hint) return -1;
	if (4 - n_blanks < hint) return -1;

	if (n_lines == hint) {
		if (AT(3) == 0)  SET_B(3);
		if (AT(5) == 0)  SET_B(5);
		if (AT(6) == 0)  SET_B(6);
		if (AT(8) == 0)  SET_B(8);
	}
	if (4 - n_blanks == hint) {
		if (AT(3) == 0)  SET_L(3);
		if (AT(5) == 0)  SET_L(5);
		if (AT(6) == 0)  SET_L(6);
		if (AT(8) == 0)  SET_L(8);
	}

	for (int i = 0; i < 12; ++i) if (AT(i) == 3) return -1;
	return ret;
}
}
