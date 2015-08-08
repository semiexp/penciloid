function UnionFind(len) {
	this.value = new Array;
	for (i = 0; i < len; ++i) this.value.push(-1);
}

UnionFind.prototype.root = function (v) {
	return this.value[v] < 0 ? v : (this.root(this.value[v]));
}

UnionFind.prototype.join = function (u, v) {
	u = this.root(u);
	v = this.root(v);

	if (u == v) return false;

	// this.value[u] += this.value[v];
	this.value[v] = u;
	return true;
}