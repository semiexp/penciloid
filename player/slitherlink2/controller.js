function Controller() {
	this.buttons = [];
	this.currentButton = -1;
}

Controller.prototype.height = 30;

Controller.prototype.setCanvas = function (canvas) { this.canvas = canvas; this.ctx = canvas.getContext("2d"); }
Controller.prototype.setApplet = function (applet) { this.applet = applet; }

Controller.prototype.addButton = function (x, y, width, height, src, action) {
	var self = this;
	var img = new Image();

	var button_elem = {
		x: x,
		y: y,
		width: width,
		height: height,
		action: action,
		img: img,
		isLoaded: false,
		drawQuery: false
	};

	img.button = button_elem;
	img.onload = function () {
		if (this.button.drawQuery) {
			if (self.ctx) self.ctx.drawImage(this, this.button.x, this.button.y);
			this.button.drawQuery = false;
		}
		this.button.isLoaded = true;
	};
	img.src = src;

	this.buttons.push(button_elem);
}

Controller.prototype.drawPanel = function () {
	this.ctx.fillStyle = "#cccccc";
	this.ctx.strokeStyle = "#cccccc";
	this.ctx.fillRect(0, 0, this.canvas.width, this.height);

	for (i = 0; i < this.buttons.length; ++i) {
		this.drawButtonFrameByID(i, false);
		if (this.buttons[i].isLoaded) this.ctx.drawImage(this.buttons[i].img, this.buttons[i].x, this.buttons[i].y);
		else this.buttons[i].drawQuery = true;
	}
}

Controller.prototype.drawButtonFrame = function (x, y, width, height, pushed) {
	var ctx = this.ctx;

	ctx.strokeStyle = pushed ? "#000000" : "#ffffff";
	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.moveTo(x + 0.5, y + height + 0.5);
	ctx.lineTo(x + 0.5, y + 0.5);
	ctx.lineTo(x + width + 0.5, y + 0.5);
	ctx.stroke();

	ctx.strokeStyle = pushed ? "#ffffff" : "#000000";
	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.moveTo(x + width + 0.5, y + 0.5);
	ctx.lineTo(x + width + 0.5, y + height + 0.5);
	ctx.lineTo(x + 0.5, y + height + 0.5);
	ctx.stroke();
}

Controller.prototype.drawButtonFrameByID = function (id, pushed) {
	this.drawButtonFrame(this.buttons[id].x, this.buttons[id].y, this.buttons[id].width, this.buttons[id].height, pushed);
}

Controller.prototype.getButtonID = function (x, y) {
	for (i = 0; i < this.buttons.length; ++i) {
		if (this.buttons[i].x <= x && x < this.buttons[i].x + this.buttons[i].width && this.buttons[i].y <= y && y < this.buttons[i].y + this.buttons[i].height) return i;
	}
	return -1;
}

Controller.prototype.mouseDown = function (x, y, b) {
	var buttonID = this.getButtonID(x, y);

	if (buttonID != -1) {
		this.currentButton = buttonID;
		this.drawButtonFrameByID(buttonID, true);
	}
}

Controller.prototype.mouseMove = function (x, y, b) {
	var buttonID = this.getButtonID(x, y);

	if (this.currentButton != -1 && buttonID == -1) {
		this.drawButtonFrameByID(this.currentButton, false);
		currentButton = -1;
	}
}

Controller.prototype.mouseUp = function (x, y, b) {
	if (this.currentButton != -1) {
		this.drawButtonFrameByID(this.currentButton, false);
		this.buttons[this.currentButton].action();
		this.currentButton = -1;
	}
}
