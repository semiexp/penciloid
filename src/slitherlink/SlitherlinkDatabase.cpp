#include "SlitherlinkDatabase.h"

namespace Penciloid
{
int *SlitherlinkDatabase::database = nullptr;
const int SlitherlinkDatabase::DATABASE_DX[] = { -2, -2, -1, -1, -1, 0, 0, 1, 1, 1, 2, 2 };
const int SlitherlinkDatabase::DATABASE_DY[] = { -1, 1, -2, 0, 2, -1, 1, -2, 0, 2, -1, 1 };

void SlitherlinkDatabase::CreateDatabase()
{
	const int pow3[] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441};

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

void SlitherlinkDatabase::ReleaseDatabase()
{
	if (database) {
		delete[] database;
		database = nullptr;
	}
}

}
