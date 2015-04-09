#include "UnionFind.h"

namespace Penciloid
{
UnionFind::UnionFind() : size(0), parent(nullptr)
{
}

UnionFind::UnionFind(int size) : size(size)
{
	parent = new int[size];
	for (int i = 0; i < size; ++i) parent[i] = -1;
}

UnionFind::~UnionFind()
{
	if (parent) delete[] parent;
}

bool UnionFind::Join(int p, int q)
{
	p = Root(p);
	q = Root(q);

	if (p == q) return false;

	if (parent[p] > parent[q]) {
		parent[q] += parent[p];
		parent[p] = q;
	} else {
		parent[p] += parent[q];
		parent[q] = p;
	}

	return true;
}

}
