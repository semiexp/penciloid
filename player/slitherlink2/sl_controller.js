function SlitherlinkController() {
	var buttons = {
		undo: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACgSURBVDhPrZQ9DsMgDEZxr5C1Y47GITv2IBm7Nldw+ZCzpDj4s/IkC9lCD/MjRFXLnTxsvA1aKCKKsPQPSgiR1mqZA84wEn1qrfp9rkhaaTwv1OHR2f56W8VnKmRk4FLIyoArzMjAUJiVAbfDjAwMhe36ZflslnG4HWalrhBkpJdCwEqnQsBIQ0IQlbYHx32weKMYsUAvnKCFM8JbjlHKD1VCgd/Tnb8vAAAAAElFTkSuQmCC",
		undo2: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACRSURBVDhP5ZQxDoAgDEWpZ/JGjl7Q1Us46TVcq98UklKQBIiLLyGk/OalA0DM7HoyyN6N74VExFhSGuL8VYjGad6ksqTyrNA378cpJ5pcnhTWyoARtsiAErbKQBD2kAE1Yam5lIMgvJ8grcsolaWUe9SEPaRKCFqlRghapEkhqJUW/0NcF+wQPAcRcf67D9a5C2UHg8+gH7nQAAAAAElFTkSuQmCC",
		redo: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACFSURBVDhPzdSxDYAwDETRmBVYMUNSMgglNSsYDpmCIvEdSsGXIuTmKSgQc/cysimew2qCZuZYMdJ1d+i13nCMVOkrq2gKHssqodShKCgFIhalQcSgEogyVAYR0FafwHnfyvXLWoyvZLCHIQnMMESDDIYokMVQCioYal6wz3emYOjvN3YpJ0uuTerIswpnAAAAAElFTkSuQmCC",
		redo2: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACPSURBVDhP5ZTBDYAgDEVbZ3Ijjy7o1SU86Rpe0W+KiRRoBDz5EkKaT18gBNg5Ry3pZG7Gd0JmdhhSKqzc89jhMC5Xo5QKKwfqyLVSJVy3Pdtk5dFLqZFGhaBUmhSCEmlWCNCUI8xN4Tz1dD5PllIR5lnhWxlICktkICoslQElrJGB+z/0V59abOWe332wRAehf35xWimMFQAAAABJRU5ErkJggg==",
		prev: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAABQSURBVDhPY/z//z8DNQETlKYaGDWQckB1A+HJhpGRkaL0AzSHEUSjGHjmXQqYTSowEZqD28CXP3aC+aQAb6nHcANHcqRQC4zAnDLiDGRgAADJsisRmkSm0QAAAABJRU5ErkJggg==",
		next: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACZSURBVDhPY/z//z8DNQETlKYawOtCRkZGrJJAPYxQJgYgaOCZdylQHgSYCM3BayBRXn75YyccEwJUD0PaGQgKL3QMlcIA+NTCIwUkiB4BIEBMuHlLPYZHFIaXkSOAGMPQweCPFJQwBDPQwNZnslAWBIDCCxvACEOQADqGSmEAfGoHfxjSLlKwAVwRhRxm6ACvgeQAKnuZgQEA9jJvQwyJUT4AAAAASUVORK5CYII=",
		zoomin: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAC8SURBVDhPtZVrEoMgDIRJD9cr2LO0v3uXXsFeDhNqmFQ2xEHdGQaM8LEhPijnnM4UBBJRE+R5tA67uq19FYKJvPhWfw51keO69JHT6hDBFCLSeOS0AHvOrPZAmzM8KlSU2tB1pAYoaWlD15GuSZl3L/lEael9nY9UHSKoTVPj83Mqvafm1eOF7mFZ2P39gS5RUeBEiVvI9/WAGw99bSxs63Soyj2nw4+NBx0GiiBUzvBo4+pz9xuf/AtIaQHeSqfrYZ8BOgAAAABJRU5ErkJggg==",
		zoomout: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAC3SURBVDhPtZRhDsMgCIWlh+sV1rN0v3eXXmG7nAUnxlUQg92XGA2xjwe0hRhjuBNREACaIN6DfOyy5L0giRFa/MqPQ35IcZ12y2lx2BMjOG45TYKWGDMi2vRwFmko6hohDQUvD5VMsDDeFTP8p2TOZpVluSOKQ0uU4+/9kXaN5tPDB9VG1mLr6xAzNz3UyqF4LfJ5bmJi19+mFrs6dU2559T92miibkFCFKUezi6cPm7fs2soOiGcuYWc/AG0m0MAAAAASUVORK5CYII="
	};

	this.zoom = 5;

	var self = this;

	this.controller = new Controller;
	this.controller.addButton(5, 5, 20, 20, buttons.undo2, function () { while (self.board.doUndo()); });
	this.controller.addButton(30, 5, 20, 20, buttons.undo, function () { self.board.doUndo(); });
	this.controller.addButton(55, 5, 20, 20, buttons.redo, function () { self.board.doRedo(); });
	this.controller.addButton(80, 5, 20, 20, buttons.redo2, function () { while (self.board.doRedo()); });
	this.controller.addButton(110, 5, 20, 20, buttons.zoomin, function () { self.zoomIn(1); });
	this.controller.addButton(135, 5, 20, 20, buttons.zoomout, function () { self.zoomIn(-1); });
	this.controller.addButton(165, 5, 20, 20, buttons.prev, function () { self.setProblemID((self.problemID - 1 + self.problems.length) % self.problems.length); });
	this.controller.addButton(190, 5, 20, 20, buttons.next, function () { self.setProblemID((self.problemID + 1 + self.problems.length) % self.problems.length); });
}

SlitherlinkController.prototype.setBoard = function (board) { this.board = board; }
SlitherlinkController.prototype.setProblems = function (problems) { this.problems = problems; }
SlitherlinkController.prototype.setProblemID = function (id) {
	this.problemID = id;
	var field = new Field;
	field.loadProblemCode(this.problems[id]);
	this.board.setField(field);
	this.drawPanel();
}
SlitherlinkController.prototype.zoomIn = function (ofs) {
	this.zoom += ofs;
	if (this.zoom < 2) this.zoom = 2;
	if (this.zoom > 10) this.zoom = 10;

	this.board.setZoom(this.zoom);
}

SlitherlinkController.prototype.height = Controller.prototype.height;
SlitherlinkController.prototype.setCanvas = function (canvas) { this.controller.setCanvas(canvas); this.ctx = canvas.getContext("2d"); }
SlitherlinkController.prototype.setApplet = function (applet) { this.controller.setApplet(applet); }
SlitherlinkController.prototype.drawPanel = function () {
	this.controller.drawPanel();

	this.ctx.fillStyle = "#000000";
	this.ctx.font = 20 + "px 'Consolas'";
	this.ctx.textAlign = "left";
	this.ctx.textBaseline = "alphabetic";

	if (this.problems) this.ctx.fillText((this.problemID + 1) + "/" + this.problems.length, 215, 25);
}
SlitherlinkController.prototype.mouseDown = function (x, y, b) { this.controller.mouseDown(x, y, b); }
SlitherlinkController.prototype.mouseMove = function (x, y, b) { this.controller.mouseMove(x, y, b); }
SlitherlinkController.prototype.mouseUp = function (x, y, b) { this.controller.mouseUp(x, y, b); }
