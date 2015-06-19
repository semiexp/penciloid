#include "SlitherlinkProblem.h"
#include <string>

namespace Penciloid
{
SlitherlinkProblem::SlitherlinkProblem()
{
	hint = nullptr;
}
SlitherlinkProblem::~SlitherlinkProblem()
{
	if (hint) delete[] hint;
}
void SlitherlinkProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	hint = new int[height * width];

	for (int i = 0; i < height * width; ++i) hint[i] = -1;
}
void SlitherlinkProblem::Init(int height_t, int width_t, int *hint_t)
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t * width_t; ++i) {
		if (0 <= hint[i] && hint[i] <= 3) hint[i] = hint_t[i];
	}
}
void SlitherlinkProblem::Init(int height_t, int width_t, char *hint_t[])
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t; ++i) {
		for (int j = 0; j < width_t; ++j) {
			if ('0' <= hint_t[i][j] && hint_t[i][j] <= '3') {
				SetHint(i, j, hint_t[i][j] - '0');
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
		ret.push_back(converter((hint[i] + 1) + (i + 1 < height * width ? (hint[i + 1] + 1) : 0) * 5));
	}

	return ret;
}

}