#pragma once

namespace Penciloid
{
template <typename T, const int SIZE>
class MiniVector
{
public:
	MiniVector<T, SIZE>() : loc(0) { }

	void push_back(const T &data) { storage[loc++] = data; }
	void clear() { loc = 0; }
	int size() const { return loc; }
	T &operator[](int i) { return storage[i]; }
	T* begin() { return storage; }
	T* end() { return &(storage[loc]); }

private:
	T storage[SIZE];
	int loc;
};

}
