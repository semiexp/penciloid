#pragma once

namespace Penciloid
{
class UnionFind
{
public:
	UnionFind();
	UnionFind(int size);
	~UnionFind();

	int Root(int p) { return parent[p] < 0 ? p : (parent[p] = Root(parent[p])); }
	inline bool Join(int p, int q);
	inline int UnionSize(int p) { return -parent[Root(p)]; }

private:
	int size;
	int *parent;
};

}
