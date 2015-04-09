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
	bool Join(int p, int q);

private:
	int size;
	int *parent;
};

}
