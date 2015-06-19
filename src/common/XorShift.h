namespace Penciloid
{
class XorShift
{
public:
	XorShift() : x(123456789), y(362436069), z(521288629), w(88675123) { }

	XorShift(int seed) : x(123456789), y(362436069), z(521288629), w(88675123) {
		x ^= seed;
		y ^= rotate_right(seed, 8);
		z ^= rotate_right(seed, 24);
		w ^= rotate_right(seed, 16);
	}

	unsigned int NextInt()
	{
		unsigned int t = x ^ (x << 11);
		x = y; y = z; z = w;
		w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
		return w & 0x7fffffff;
	}

private:
	unsigned int rotate_right(unsigned int v, int r) { return (v >> r) | ((v & ((1 << r) - 1)) << (32 - r)); }

	int x, y, z, w;
};
}
