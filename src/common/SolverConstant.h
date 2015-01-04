#pragma once

namespace Penciloid
{

class SolverStatus
{
public:
	static const int NORMAL = 0;
	static const int SUCCESS = 1;
	static const int INCONSISTENT = 2;
	static const int NOT_UNIQUE = 4;
	static const int UNEXPECTED = 0x80000000;
};

class GridConstant
{
public:
	static const int GRID_DY[4];
	static const int GRID_DX[4];
};

};
