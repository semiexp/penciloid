#pragma once

namespace Penciloid
{
template <typename T, int size>
class MiniVector
{
public:
	MiniVector<T, size>() : loc(0) { }

	void push_back(T &data) { storage[loc++] = data; }
	void clear() { loc = 0; }
	int size() { return loc; }
	T &operator[](int i) { return storage[i]; }
	T* begin() { return storage; }
	T* end() { return &(storage[loc]); }

private:
	T storage[size];
	int loc;
};

}
