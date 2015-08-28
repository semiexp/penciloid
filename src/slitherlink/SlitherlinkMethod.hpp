#pragma once

#include "../common/GridLoopMethod.hpp"

namespace Penciloid
{
class SlitherlinkMethod
{
public:
	SlitherlinkMethod() : grid_loop_method(), check_cell(true), adjacent_3(true), diagonal_3(true), diagonal_chain(true) {}

	GridLoopMethod grid_loop_method;
	bool check_cell, adjacent_3, diagonal_3, diagonal_chain;
};

}
