function Field() {
	this.height = 0;
	this.width = 0;
	this.clues = [];
	this.lines = [];
}

Field.undecided = 0;
Field.line = 1;
Field.blank = 2;

Field.prototype.loadProblemCode = function (problem) {
	function CharToInt(a) {
		var id = a.charCodeAt(0);
		if (48 <= id && id <= 57) return id - 48;
		if (97 <= id && id <= 122) return id - 87;
		if (65 <= id && id <= 90) return id - 29;
	}
	this.height = CharToInt(problem.charAt(0));
	this.width = CharToInt(problem.charAt(1));

	// TODO: set problem appropriately
	this.clues = [];
	for (var y = 0; y < this.height; ++y) {
		var line = [];
		for (var x = 0; x < this.width; ++x) {
			var idx = y * this.width + x;
			if (idx % 2 == 0) line.push(CharToInt(problem.charAt(Math.floor(idx / 2) + 2)) % 5 - 1);
			if (idx % 2 == 1) line.push(Math.floor(CharToInt(problem.charAt(Math.floor(idx / 2) + 2)) / 5) - 1);
		}
		this.clues.push(line);
	}

	this.lines = [];
	for (var y = 0; y <= this.height * 2; ++y) {
		var line = [];
		for (var x = 0; x <= this.width * 2; ++x) line.push(Field.undecided);
		this.lines.push(line);
	}
}

Field.prototype.getClue = function (y, x) {
	return this.clues[y][x];
}

Field.prototype.getLineStyle = function (y, x) {
	return this.lines[y][x];
}

Field.prototype.setLineStyle = function (y, x, u) {
	this.lines[y][x] = u;
}

Field.prototype.isComplete = function () {
	var height = this.height, width = this.width;

	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			var aroundLines =
				(this.getLineStyle(y * 2 + 0, x * 2 + 1) == Field.line ? 1 : 0) +
				(this.getLineStyle(y * 2 + 1, x * 2 + 0) == Field.line ? 1 : 0) +
				(this.getLineStyle(y * 2 + 2, x * 2 + 1) == Field.line ? 1 : 0) +
				(this.getLineStyle(y * 2 + 1, x * 2 + 2) == Field.line ? 1 : 0);
			
			var clue = this.getClue(y, x);
			if (0 <= clue && clue <= 3 && clue != aroundLines) return false;
		}
	}

	function id(x, y) { return y * (width * 2 + 1) + x; }

	var uf = new UnionFind((height * 2 + 1) * (width * 2 + 1));

	for (var y = 0; y <= height * 2; y += 2) {
		for (var x = 0; x <= width * 2; x += 2) {
			var aroundLines = 0;
			if (y > 0 && this.getLineStyle(y - 1, x) == Field.line) {
				uf.join(id(x, y), id(x, y - 1));
				++aroundLines;
			}
			if (x > 0 && this.getLineStyle(y, x - 1) == Field.line) {
				uf.join(id(x, y), id(x - 1, y));
				++aroundLines;
			}
			if (y < height * 2 && this.getLineStyle(y + 1, x) == Field.line) {
				uf.join(id(x, y), id(x, y + 1));
				++aroundLines;
			}
			if (x < width * 2 && this.getLineStyle(y, x + 1) == Field.line) {
				uf.join(id(x, y), id(x + 1, y));
				++aroundLines;
			}

			if (aroundLines != 0 && aroundLines != 2) return false;
		}
	}

	var lineRoot = -1;
	for (var y = 0; y <= height * 2; ++y) {
		for (var x = 0; x <= width * 2; ++x) {
			if (y % 2 != x % 2 && this.getLineStyle(y, x) == Field.line) {
				if (lineRoot == -1) lineRoot = uf.root(id(x, y));
				else if (lineRoot != uf.root(id(x, y))) return false;
			}
		}
	}

	return true;
}
