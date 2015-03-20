#include "BlackCellForest.h"

namespace Penciloid
{
BlackCellForest::BlackCellForest() : group_root(nullptr), is_black(nullptr), height(0), width(0), out_of_range(0)
{
}

BlackCellForest::~BlackCellForest()
{
	if (group_root) delete[] group_root;
	if (is_black) delete[] is_black;
}

void BlackCellForest::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (group_root) delete[] group_root;
	if (is_black) delete[] is_black;

	group_root = new int[height * width + 1];
	is_black = new bool[height * width + 1];
	out_of_range = height * width;

	for (int i = 0; i <= height * width; ++i) group_root[i] = -1;
	for (int i = 0; i < height * width; ++i) is_black[i] = false;

	is_black[out_of_range] = true;
}

void BlackCellForest::Join(int u, int v)
{
	u = CellRoot(u);
	v = CellRoot(v);
	if (u == v) return;

	group_root[u] += group_root[v];
	group_root[v] = u;
}

void BlackCellForest::Blacken(int y, int x)
{
	int id = CellId(y, x);

	if (is_black[id]) return;
	is_black[id] = true;

	JoinIfBlackened(id, CellVirtualId(y - 1, x - 1));
	JoinIfBlackened(id, CellVirtualId(y - 1, x + 1));
	JoinIfBlackened(id, CellVirtualId(y + 1, x - 1));
	JoinIfBlackened(id, CellVirtualId(y + 1, x + 1));
}

bool BlackCellForest::IsArticulationPoint(int y, int x)
{
	if (y > 0) {
		if (VirtualCellRoot(y - 1, x - 1) == VirtualCellRoot(y - 1, x + 1)) return true;
	}
	if (y < height - 1) {
		if (VirtualCellRoot(y + 1, x - 1) == VirtualCellRoot(y + 1, x + 1)) return true;
	}
	if (x > 0) {
		if (VirtualCellRoot(y - 1, x - 1) == VirtualCellRoot(y + 1, x - 1)) return true;
	}
	if (x < width - 1) {
		if (VirtualCellRoot(y - 1, x + 1) == VirtualCellRoot(y + 1, x + 1)) return true;
	}
	if ((y != 0 && x != width - 1) || (y != height - 1 && x != 0)) {
		if (VirtualCellRoot(y - 1, x - 1) == VirtualCellRoot(y + 1, x + 1)) return true;
	}
	if ((y != 0 && x != 0) || (y != height - 1 && x != width - 1)) {
		if (VirtualCellRoot(y - 1, x + 1) == VirtualCellRoot(y + 1, x - 1)) return true;
	}

	return false;
}

}
