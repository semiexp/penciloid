#include "SlitherlinkProblem.h"
#include <string>

namespace Penciloid
{
SlitherlinkProblem::SlitherlinkProblem()
{
	clue = nullptr;
}
SlitherlinkProblem::~SlitherlinkProblem()
{
	if (clue) delete[] clue;
}
void SlitherlinkProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (clue) delete[] clue;
	clue = new int[height * width];

	for (int i = 0; i < height * width; ++i) clue[i] = -1;
}
void SlitherlinkProblem::Init(int height_t, int width_t, int *clue_t)
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t * width_t; ++i) {
		if (0 <= clue[i] && clue[i] <= 3) clue[i] = clue_t[i];
	}
}
void SlitherlinkProblem::Init(int height_t, int width_t, char *clue_t[])
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t; ++i) {
		for (int j = 0; j < width_t; ++j) {
			if ('0' <= clue_t[i][j] && clue_t[i][j] <= '3') {
				SetClue(i, j, clue_t[i][j] - '0');
			}
		}
	}
}
std::string SlitherlinkProblem::GetPlayerFormatString()
{
	auto converter = [](int a){
		if (a < 10) return (char)(a + '0');
		if (a < 36) return (char)(a - 10 + 'a');
		return (char)(a - 36 + 'A');
	};

	std::string ret;

	ret.push_back(converter(GetHeight()));
	ret.push_back(converter(GetWidth()));

	for (int i = 0; i < height * width; i += 2) {
		ret.push_back(converter((clue[i] + 1) + (i + 1 < height * width ? (clue[i + 1] + 1) : 0) * 5));
	}

	return ret;
}

}