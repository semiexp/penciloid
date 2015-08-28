#pragma once

#include "../common/GridLoopMethod.hpp"

namespace Penciloid
{
class SlitherlinkMethod
{
public:
	SlitherlinkMethod() : grid_loop_method(), check_cell(true), diagonal_chain(true) {}

	GridLoopMethod grid_loop_method;
	bool check_cell, diagonal_chain;
};

}
