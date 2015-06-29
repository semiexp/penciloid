#include <cassert>
#include <cstdio>

#include "../common/GridLoop.hpp"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::GridLoopTest1()
{
	GridLoopSimple grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 0);

	assert(grid.GetSegmentStyle(2, 1) == grid.LOOP_BLANK);
	assert(grid.GetSegmentStyle(3, 0) == grid.LOOP_LINE);
}
void PenciloidTest::GridLoopTest2a()
{
	GridLoopSimple grid;
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
	GridLoopSimple grid;
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
	GridLoopSimple grid;
	grid.Init(3, 3);

	grid.DetermineLine(0, 1);
	grid.DetermineLine(2, 1);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 2);

	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest4a()
{
	GridLoopSimple grid;
	grid.Init(3, 3);

	grid.DetermineLine(5, 6);
	grid.DetermineLine(1, 4);
	grid.DetermineLine(2, 3);
	grid.DetermineLine(0, 1);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(3, 0) == GridLoopSimple::LOOP_LINE);
}
void PenciloidTest::GridLoopTest4b()
{
	GridLoopSimple grid;
	grid.Init(3, 3);

	grid.DetermineLine(1, 4);
	grid.DetermineLine(2, 3);
	grid.DetermineLine(0, 1);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(3, 0) == GridLoopSimple::LOOP_UNDECIDED);
}
void PenciloidTest::GridLoopTest5a()
{
	GridLoopSimple grid;
	grid.Init(4, 3);

	grid.DetermineLine(3, 2);
	grid.DetermineLine(3, 4);
	grid.DetermineLine(4, 1);
	grid.DetermineLine(4, 5);
	grid.DetermineLine(6, 3);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(0, 3);
	grid.DetermineLine(0, 5);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(2, 3) == GridLoopSimple::LOOP_BLANK);
}
void PenciloidTest::GridLoopTest5b()
{
	GridLoopSimple grid;
	grid.Init(4, 3);

	grid.DetermineLine(3, 2);
	grid.DetermineLine(3, 4);
	grid.DetermineLine(4, 1);
	grid.DetermineLine(4, 5);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(0, 3);
	grid.DetermineLine(0, 5);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(2, 3) == GridLoopSimple::LOOP_UNDECIDED);
}

}