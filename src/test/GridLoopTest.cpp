#include <cassert>
#include <cstdio>

#include "../common/GridLoop.hpp"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::GridLoopTest1()
{
	GridLoop<LoopNullVertex, LoopNullCell> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 0);

	assert(grid.GetSegmentStyle(2, 1) == grid.LOOP_BLANK);
	assert(grid.GetSegmentStyle(3, 0) == grid.LOOP_LINE);
}
void PenciloidTest::GridLoopTest2a()
{
	GridLoop<LoopNullVertex, LoopNullCell> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineBlank(3, 0);
	grid.DetermineLine(0, 1);

	assert(grid.GetStatus() & SolverStatus::SUCCESS);
	grid.DetermineLine(4, 1);
	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest2b()
{
	GridLoop<LoopNullVertex, LoopNullCell> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(2, 1);

	assert(grid.GetStatus() & SolverStatus::SUCCESS);
	grid.DetermineLine(4, 1);
	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest3()
{
	GridLoop<LoopNullVertex, LoopNullCell> grid;
	grid.Init(3, 3);

	grid.DetermineLine(0, 1);
	grid.DetermineLine(2, 1);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 2);

	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}

}