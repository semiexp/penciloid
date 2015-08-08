function Board() {
	// settings
	this.dotSize = 5;
	this.hintSize = 30;
	this.unitSize = this.dotSize + this.hintSize;
	this.marginX = 20;
	this.marginY = 20;
	this.controllerHeight = 30;
	this.lineLength = 28;
	this.lineWidth = 3;
	this.blankXSize = 8;
	this.allowedDistance = 5;
	this.motionlessDistance = 3;

	this.currentStyle = -1;
	this.isComplete = false;
	this.undoHistory = [];
	this.redoHistory = [];
}

Board.prototype.setCanvas = function (canvas) { this.canvas = canvas; this.ctx = canvas.getContext("2d"); }
Board.prototype.setApplet = function (applet) { this.applet = applet; }

Board.prototype.setField = function (field) {
	this.field = field;
	this.isComplete = false;
	this.undoHistory = [];
	this.redoHistory = [];
	var totalWidth = 2 * this.marginX + field.width * this.unitSize + this.dotSize;
	var totalHeight = 2 * this.marginX + field.height * this.unitSize + this.dotSize;

	this.applet.resize(totalWidth, totalHeight + this.controllerHeight);
	if (this.ctx) this.drawPanel();
}

Board.prototype.drawPanel = function () {
	var field = this.field;
	var ctx = this.ctx;

	if (!field) return;

	var totalWidth = 2 * this.marginX + field.width * this.unitSize + this.dotSize;
	var totalHeight = 2 * this.marginX + field.height * this.unitSize + this.dotSize;

	ctx.fillStyle = "#ffffff";
	ctx.lineWidth = 1.0;
	ctx.fillRect(1, this.controllerHeight, totalWidth, totalHeight);

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#666666";
	ctx.lineWidth = 1;
	ctx.strokeRect(1, this.controllerHeight, totalWidth - 2, totalHeight - 2);

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#000000";
	var offsetX = this.marginX;
	var offsetY = this.marginY + this.controllerHeight;
	for (y = 0; y <= field.height; ++y) {
		for (x = 0; x <= field.width; ++x) {
			ctx.fillRect(offsetX + x * this.unitSize, offsetY + y * this.unitSize , this.dotSize, this.dotSize);
		}
	}

	ctx.font = this.hintSize + "px 'Consolas'";
	ctx.textAlign = "center";
	ctx.textBaseline = "bottom";
	for (var y = 0; y < field.height; ++y) {
		for (var x = 0; x < field.width; ++x) {
			var clue = field.getClue(y, x);
			if (0 <= clue && clue <= 3) {
				ctx.fillText(clue,
					offsetX + x * this.unitSize + this.dotSize + this.hintSize / 2,
					offsetY + (y + 1) * this.unitSize,
					this.hintSize
					);
			}
		}
	}
}

Board.prototype.showComplete = function (type) {
	var totalWidth = 2 * this.marginX + this.field.width * this.unitSize + this.dotSize;
	var totalHeight = 2 * this.marginX + this.field.height * this.unitSize + this.dotSize;

	this.ctx.lineWidth = 2;
	this.ctx.strokeStyle = type ? "#ff0000" : "#ffffff";
	this.ctx.strokeRect(4, 3 + this.controllerHeight, totalWidth - 8, totalHeight - 8);
}

Board.prototype.updatePanel = function (tx, ty) {
	// update around line (ty, tx)
	var ctx = this.ctx;

	ctx.fillStyle = "#ffffff";
	ctx.strokeStyle = "#ffffff";
	var clearWidth = Math.max(this.dotSize, this.blankXSize) + 2;
	var offsetX = this.marginX;
	var offsetY = this.marginY + this.controllerHeight;

	if (ty % 2 == 1 && tx % 2 == 0) {
		ctx.fillRect(
			offsetX + Math.floor(tx / 2) * this.unitSize - (clearWidth - this.dotSize) / 2,
			offsetY + Math.floor(ty / 2) * this.unitSize + this.dotSize,
			clearWidth,
			this.hintSize
		);
	} else if (ty % 2 == 0 && tx % 2 == 1) {
		ctx.fillRect(
			offsetX + Math.floor(tx / 2) * this.unitSize + this.dotSize,
			offsetY + Math.floor(ty / 2) * this.unitSize - (clearWidth - this.dotSize) / 2,
			this.hintSize,
			clearWidth
		);
	}

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#000000";

	if (this.field.getLineStyle(ty, tx) == Field.line) {
		if (ty % 2 == 1 && tx % 2 == 0) {
			ctx.fillRect(
				offsetX + Math.floor(tx / 2) * this.unitSize + (this.dotSize - this.lineWidth) / 2,
				offsetY + Math.floor(ty / 2) * this.unitSize + this.dotSize + (this.hintSize - this.lineLength) / 2,
				this.lineWidth,
				this.lineLength
			);
		} else if (ty % 2 == 0 && tx % 2 == 1) {
			ctx.fillRect(
				offsetX + Math.floor(tx / 2) * this.unitSize + this.dotSize + (this.hintSize - this.lineLength) / 2,
				offsetY + Math.floor(ty / 2) * this.unitSize + (this.dotSize - this.lineWidth) / 2,
				this.lineLength,
				this.lineWidth
			);
		}
	} else if (this.field.getLineStyle(ty, tx) == Field.blank) {
		var cx, cy;

		if (ty % 2 == 1 && tx % 2 == 0) {
			cx = offsetX + Math.floor(tx / 2) * (this.dotSize + this.hintSize) + this.dotSize / 2;
			cy = offsetY + Math.floor(ty / 2) * (this.dotSize + this.hintSize) + this.dotSize + this.hintSize / 2;
		} else if (ty % 2 == 0 && tx % 2 == 1) {
			cx = offsetX + Math.floor(tx / 2) * (this.dotSize + this.hintSize) + this.dotSize + this.hintSize / 2;
			cy = offsetY + Math.floor(ty / 2) * (this.dotSize + this.hintSize) + this.dotSize / 2;
		}

		ctx.beginPath();
		ctx.moveTo(cx - this.blankXSize / 2, cy - this.blankXSize / 2);
		ctx.lineTo(cx + this.blankXSize / 2, cy + this.blankXSize / 2);
		ctx.moveTo(cx + this.blankXSize / 2, cy - this.blankXSize / 2);
		ctx.lineTo(cx - this.blankXSize / 2, cy + this.blankXSize / 2);
		ctx.closePath();
		ctx.stroke();
	}
}

Board.prototype.locateMouse = function (x, y) {
	x -= this.marginX; y -= this.marginY;
	var cellX = Math.floor(x / this.unitSize);
	var cellY = Math.floor(y / this.unitSize);

	var relX = x - cellX * this.unitSize;
	var relY = y - cellY * this.unitSize;

	if (
		(relX < this.dotSize + this.motionlessDistance || this.unitSize - this.motionlessDistance < relX) &&
		(relY < this.dotSize + this.motionlessDistance || this.unitSize - this.motionlessDistance < relY)
		) return { x: -1, y: -1 };

	if (relX <= this.dotSize) return { x: 2 * cellX, y: 2 * cellY + 1 };
	if (relY <= this.dotSize) return { x: 2 * cellX + 1, y: 2 * cellY };

	var nearX = 2 * cellX + 1, nearY = 2 * cellY, distance = relY - this.dotSize;

	if (distance > relX - this.dotSize) {
		nearX = 2 * cellX;
		nearY = 2 * cellY + 1;
		distance = relX - this.dotSize;
	}

	if (distance > this.hintSize + this.dotSize - relY) {
		nearX = 2 * cellX + 1;
		nearY = 2 * cellY + 2;
		distance = this.hintSize + this.dotSize - relY;
	}

	if (distance > this.hintSize + this.dotSize - relX) {
		nearX = 2 * cellX + 2;
		nearY = 2 * cellY + 1;
		distance = this.hintSize + this.dotSize - relX;
	}

	if (distance > this.allowedDistance) return { x: -1, y: -1 };
	return { x: nearX, y: nearY };
}

Board.prototype.updateField = function (x, y, st, setUndo) {
	if (this.field.getLineStyle(y, x) == st) return;

	if (setUndo) {
		this.redoHistory = [];
		this.undoHistory.push({ x: x, y: y, style: this.field.getLineStyle(y, x) });
	}

	this.field.setLineStyle(y, x, st);
	this.updatePanel(x, y);

	if (this.field.isComplete() != this.isComplete) {
		this.isComplete = !this.isComplete;
		this.showComplete(this.isComplete);
	}
}

Board.prototype.doUndo = function () {
	if (this.undoHistory.length == 0) return false;

	var undoData = this.undoHistory[this.undoHistory.length - 1]; this.undoHistory.pop();
	this.redoHistory.push({ x: undoData.x, y: undoData.y, style: this.field.getLineStyle(undoData.y, undoData.x) });
	this.updateField(undoData.x, undoData.y, undoData.style, false);
	return true;
}

Board.prototype.doRedo = function () {
	if (this.redoHistory.length == 0) return false;

	var redoData = this.redoHistory[this.redoHistory.length - 1]; this.redoHistory.pop();
	this.undoHistory.push({ x: redoData.x, y: redoData.y, style: this.field.getLineStyle(redoData.y, redoData.x) });
	this.updateField(redoData.x, redoData.y, redoData.style, false);
	return true;
}

Board.prototype.mouseDown = function (x, y, b) {
	var loc = this.locateMouse(x, y);

	if (!this.isComplete && 0 <= loc.x && loc.x <= this.field.width * 2 && 0 <= loc.y && loc.y <= this.field.height * 2) {
		this.currentStyle = (this.field.getLineStyle(loc.y, loc.x) + (b ? 2 : 1)) % 3;

		this.updateField(loc.x, loc.y, this.currentStyle, true);
	}
}

Board.prototype.mouseMove = function (x, y, b) {
	var loc = this.locateMouse(x, y);

	if (!this.isComplete && this.currentStyle != -1 && 0 <= loc.x && loc.x <= this.field.width * 2 && 0 <= loc.y && loc.y <= this.field.height * 2) {
		this.updateField(loc.x, loc.y, this.currentStyle, true);
	}
}

Board.prototype.mouseUp = function (x, y, b) {
	this.currentStyle = -1;
}
