#pragma once

#include "../common/GridLoopMethod.hpp"

namespace Penciloid
{
class SlitherlinkMethod
{
public:
	SlitherlinkMethod() : grid_loop_method(), diagonal_chain(true) {}

	GridLoopMethod grid_loop_method;
	bool diagonal_chain;
};

}
