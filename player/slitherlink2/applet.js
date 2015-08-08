function Applet(canvas, controller, board) {
	this.canvas = canvas;
	this.ctx = canvas.getContext("2d");
	this.controller = controller;
	this.board = board;
	var self = this;

	if (canvas.addEventListener) {
		canvas.addEventListener("mousedown", function (event) {
			var x = event.clientX, y = event.clientY, button = (event.button == 2);
			var rect = event.target.getBoundingClientRect();
			self.mouseDown(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("touchstart", function (event) {
			event.preventDefault();
			var x = event.changedTouches[0].clientX, y = event.changedTouches[0].clientY, button = false;
			var rect = event.target.getBoundingClientRect();
			self.mouseDown(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("mousemove", function (event) {
			var x = event.clientX, y = event.clientY, button = (event.button == 2);
			var rect = event.target.getBoundingClientRect();
			self.mouseMove(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("touchmove", function (event) {
			event.preventDefault();
			var x = event.changedTouches[0].clientX, y = event.changedTouches[0].clientY, button = false;
			var rect = event.target.getBoundingClientRect();
			self.mouseMove(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("mouseup", function (event) {
			var x = event.clientX, y = event.clientY, button = (event.button == 2);
			var rect = event.target.getBoundingClientRect();
			self.mouseUp(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("touchend", function (event) {
			event.preventDefault();
			var x = event.changedTouches[0].clientX, y = event.changedTouches[0].clientY, button = false;
			var rect = event.target.getBoundingClientRect();
			self.mouseUp(x - rect.left, y - rect.top, button);
		});
		canvas.addEventListener("contextmenu", function (e) { e.preventDefault(); });
	}

	controller.setCanvas(canvas);
	controller.setApplet(this);
	controller.drawPanel();

	board.setCanvas(canvas);
	board.setApplet(this);
	board.drawPanel();
}

Applet.prototype.resize = function (width, height) {
	if (this.canvas.width != width || this.canvas.height != height) {
		this.canvas.width = width;
		this.canvas.height = height;

		this.controller.drawPanel();
		this.board.drawPanel();
	}
}

Applet.prototype.mouseDown = function (x, y, b) {
	if (y < this.controller.height) this.controller.mouseDown(x, y, b);
	else this.board.mouseDown(x, y - this.controller.height, b);
}

Applet.prototype.mouseMove = function (x, y, b) {
	if (y < this.controller.height) this.controller.mouseMove(x, y, b);
	else this.board.mouseMove(x, y - this.controller.height, b);
}

Applet.prototype.mouseUp = function (x, y, b) {
	if (y < this.controller.height) this.controller.mouseUp(x, y, b);
	else this.board.mouseUp(x, y - this.controller.height, b);
}
