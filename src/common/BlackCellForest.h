#pragma once

namespace Penciloid
{
class BlackCellForest
{
public:
	BlackCellForest();
	~BlackCellForest();

	void Init(int height_t, int width_t);

	void Blacken(int y, int x);
	inline bool IsBlackened(int y, int x) { return is_black[CellId(y, x)]; }
	bool IsArticulationPoint(int y, int x);

private:
	inline int CellId(int y, int x) const { return y * width + x; }
	inline int CellVirtualId(int y, int x) const { return CheckRange(y, x) ? CellId(y, x) : out_of_range; }
	inline bool CheckRange(int y, int x) const { return 0 <= y && y < height && 0 <= x && x < width; }

	int CellRoot(int id) { return group_root[id] < 0 ? id : (group_root[id] = CellRoot(group_root[id])); }
	inline int VirtualCellRoot(int y, int x) { return CellRoot(CellVirtualId(y, x)); }
	void Join(int u, int v);
	void JoinIfBlackened(int u, int v) { if (is_black[u] && is_black[v]) Join(u, v); }

	int *group_root;
	bool *is_black;
	int height, width, out_of_range;

};

}