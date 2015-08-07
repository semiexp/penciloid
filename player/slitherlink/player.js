// problem
var height, width, problem;
var problem_set;
var problem_id;

// player internal status
var lines;
var next_status = -1;
var is_completed = false;
var UNDECIDED = 0, LINE = 1, BLANK = 2;

// player canvas and its status, constants
var canvas, ctx;
var player_height, player_width;

var dot_size = 5, hint_size = 30;
var margin_y = 20, margin_x = 20;
var line_height = 28, line_width = 3;
var blank_x_size = 8;
var allowed_distance = 5, motionless_distance = 3;

// control unit
var control_unit_height = 30, control_button_size = 20, control_button_margin_x = 5;
var current_button = -1;
var use_control_unit = true;

// control button
var control_buttons = [
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACRSURBVDhP5ZQxDoAgDEWpZ/JGjl7Q1Us46TVcq98UklKQBIiLLyGk/OalA0DM7HoyyN6N74VExFhSGuL8VYjGad6ksqTyrNA378cpJ5pcnhTWyoARtsiAErbKQBD2kAE1Yam5lIMgvJ8grcsolaWUe9SEPaRKCFqlRghapEkhqJUW/0NcF+wQPAcRcf67D9a5C2UHg8+gH7nQAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACgSURBVDhPrZQ9DsMgDEZxr5C1Y47GITv2IBm7Nldw+ZCzpDj4s/IkC9lCD/MjRFXLnTxsvA1aKCKKsPQPSgiR1mqZA84wEn1qrfp9rkhaaTwv1OHR2f56W8VnKmRk4FLIyoArzMjAUJiVAbfDjAwMhe36ZflslnG4HWalrhBkpJdCwEqnQsBIQ0IQlbYHx32weKMYsUAvnKCFM8JbjlHKD1VCgd/Tnb8vAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACFSURBVDhPzdSxDYAwDETRmBVYMUNSMgglNSsYDpmCIvEdSsGXIuTmKSgQc/cysimew2qCZuZYMdJ1d+i13nCMVOkrq2gKHssqodShKCgFIhalQcSgEogyVAYR0FafwHnfyvXLWoyvZLCHIQnMMESDDIYokMVQCioYal6wz3emYOjvN3YpJ0uuTerIswpnAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACPSURBVDhP5ZTBDYAgDEVbZ3Ijjy7o1SU86Rpe0W+KiRRoBDz5EkKaT18gBNg5Ry3pZG7Gd0JmdhhSKqzc89jhMC5Xo5QKKwfqyLVSJVy3Pdtk5dFLqZFGhaBUmhSCEmlWCNCUI8xN4Tz1dD5PllIR5lnhWxlICktkICoslQElrJGB+z/0V59abOWe332wRAehf35xWimMFQAAAABJRU5ErkJggg",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAABQSURBVDhPY/z//z8DNQETlKYaGDWQckB1A+HJhpGRkaL0AzSHEUSjGHjmXQqYTSowEZqD28CXP3aC+aQAb6nHcANHcqRQC4zAnDLiDGRgAADJsisRmkSm0QAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAFiUAABYlAUlSJPAAAACZSURBVDhPY/z//z8DNQETlKYawOtCRkZGrJJAPYxQJgYgaOCZdylQHgSYCM3BayBRXn75YyccEwJUD0PaGQgKL3QMlcIA+NTCIwUkiB4BIEBMuHlLPYZHFIaXkSOAGMPQweCPFJQwBDPQwNZnslAWBIDCCxvACEOQADqGSmEAfGoHfxjSLlKwAVwRhRxm6ACvgeQAKnuZgQEA9jJvQwyJUT4AAAAASUVORK5CYII="
];

// operation history
var history_undo = [], history_redo = [];

onload = Init;

function LoadProblems() {
	problem_set = document.getElementsByName("problem").item(0).value.split(",");
	if (problem_set[problem_set.length - 1] == "") problem_set.pop();
	problem_id = 0;
}

function SetProblem() {
	function CharToInt(a) {
		var id = a.charCodeAt(0);
		if (48 <= id && id <= 57) return id - 48;
		if (97 <= id && id <= 122) return id - 87;
		if (65 <= id && id <= 90) return id - 29;
	}
	arg = problem_set[problem_id];
	height = CharToInt(arg.charAt(0));
	width = CharToInt(arg.charAt(1));

	// TODO: set problem appropriately
	problem = [];
	for (var y = 0; y < height; ++y) {
		var line = [];
		for (var x = 0; x < width; ++x) {
			var idx = y * width + x;
			if (idx % 2 == 0) line.push(CharToInt(arg.charAt(Math.floor(idx / 2) + 2)) % 5 - 1);
			if (idx % 2 == 1) line.push(Math.floor(CharToInt(arg.charAt(Math.floor(idx / 2) + 2)) / 5) - 1);
		}
		problem.push(line);
	}
}

function ClearLines() {
	lines = [];
	for (var y = 0; y <= height * 2; ++y) {
		var current_line = [];
		for (var x = 0; x <= width * 2; ++x) {
			current_line.push(UNDECIDED);
		}
		lines.push(current_line);
	}
}

function Init() {
	canvas = document.getElementById("player");
	if (!canvas || !canvas.getContext) return false;

	if (canvas.addEventListener) {
		canvas.addEventListener("mousedown", MouseDown);
		canvas.addEventListener("mousemove", MouseMove);
		canvas.addEventListener("mouseup", MouseUp);
		canvas.addEventListener("contextmenu", function (e) { e.preventDefault(); });
	} else {
		canvas.onmousedown = MouseDown;
		canvas.onmousemove = MouseMove;
		canvas.onmouseup = MouseUp;
		canvas.oncontextmenu = function () { return false; }
	}

	ctx = canvas.getContext("2d");

	LoadProblems();
	InitProblem();
}

function InitProblem() {
	is_completed = false;
	SetProblem();
	ClearLines();
	InitPlayerCanvas();
}

function DrawControlButtonBorder(x_bas, y_bas, height, width, mode) {
	ctx.strokeStyle = mode ? "#000000" : "#ffffff";
	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.moveTo(x_bas + 0.5, y_bas + height + 0.5);
	ctx.lineTo(x_bas + 0.5, y_bas + 0.5);
	ctx.lineTo(x_bas + width + 0.5, y_bas + 0.5);
	ctx.stroke();

	ctx.strokeStyle = mode ? "#ffffff" : "#000000";
	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.moveTo(x_bas + width + 0.5, y_bas + 0.5);
	ctx.lineTo(x_bas + width + 0.5, y_bas + height + 0.5);
	ctx.lineTo(x_bas + 0.5, y_bas + height + 0.5);
	ctx.stroke();
}

function DrawControlButtonBorder2(bid, mode) {
	DrawControlButtonBorder(control_button_margin_x * (bid + 1) + control_button_size * bid, (control_unit_height - control_button_size) / 2, control_button_size, control_button_size, mode);
}

function DrawControlUnit() {
	for (i = 0; i < 6; ++i) {
		DrawControlButtonBorder2(i, false);

		var img = new Image();
		img.i = i;
		img.onload = function () { ctx.drawImage(this, control_button_margin_x * (this.i + 1) + control_button_size * this.i, (control_unit_height - control_button_size) / 2); };
		img.src = control_buttons[img.i];
	}

	ctx.fillStyle = "#000000";
	ctx.font = control_button_size + "px 'Consolas'";
	ctx.textAlign = "left";
	ctx.textBaseline = "alphabetic";
	ctx.fillText((problem_id + 1) + "/" + problem_set.length, control_button_margin_x * (6 + 1) + control_button_size * 6, (control_button_size + control_unit_height) / 2);

}

function InitPlayerCanvas() {
	//clear
	player_height = 2 * margin_y + height * (dot_size + hint_size) + dot_size;
	player_width = 2 * margin_x + width * (dot_size + hint_size) + dot_size;
	canvas.height = player_height + control_unit_height;
	canvas.width = player_width;

	ctx.fillStyle = "#ffffff";
	ctx.lineWidth = 1.0;
	ctx.fillRect(0, control_unit_height, player_width, player_height);

	ctx.fillStyle = "#cccccc";
	ctx.strokeStyle = "#cccccc";
	ctx.fillRect(0, 0, player_width, control_unit_height);

	DrawControlUnit();

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#666666";
	ctx.strokeRect(1, control_unit_height, player_width - 2, player_height - 2);

	for (var y = 0; y <= height; ++y) {
		for (var x = 0; x <= width; ++x) {
			ctx.fillRect(margin_x + x * (dot_size + hint_size), margin_y + y * (dot_size + hint_size) + control_unit_height, dot_size, dot_size);
		}
	}

	ctx.font = hint_size + "px 'Consolas'";
	ctx.textAlign = "center";
	ctx.textBaseline = "bottom";
	for (var y = 0; y < height; ++y) {
		for (var x = 0; x < width; ++x) {
			if (0 <= problem[y][x] && problem[y][x] <= 3) {
				ctx.fillText(problem[y][x],
					margin_x + x * (dot_size + hint_size) + dot_size + hint_size / 2,
					margin_y + y * (dot_size + hint_size) + dot_size + hint_size + control_unit_height,
					hint_size
					);
			}
		}
	}
}

function UpdatePlayerCanvas(tx, ty) {
	// update around line (ty, tx)

	ctx.fillStyle = "#ffffff";
	ctx.strokeStyle = "#ffffff";
	var clear_width = Math.max(dot_size, blank_x_size) + 2;

	if (ty % 2 == 1 && tx % 2 == 0) {
		ctx.fillRect(
			margin_x + Math.floor(tx / 2) * (dot_size + hint_size) - (clear_width - dot_size) / 2,
			margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size + control_unit_height,
			clear_width,
			hint_size
		);
	} else if (ty % 2 == 0 && tx % 2 == 1) {
		ctx.fillRect(
			margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size,
			margin_y + Math.floor(ty / 2) * (dot_size + hint_size) - (clear_width - dot_size) / 2 + control_unit_height,
			hint_size,
			clear_width
		);
	}

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#000000";

	if (lines[ty][tx] == LINE) {
		if (ty % 2 == 1 && tx % 2 == 0) {
			ctx.fillRect(
				margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + (dot_size - line_width) / 2,
				margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size + (hint_size - line_height) / 2 + control_unit_height,
				line_width,
				line_height
			);
		} else if (ty % 2 == 0 && tx % 2 == 1) {
			ctx.fillRect(
				margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size + (hint_size - line_height) / 2 ,
				margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + (dot_size - line_width) / 2 + control_unit_height,
				line_height,
				line_width
			);
		}
	} else if (lines[ty][tx] == BLANK) {
		var cx, cy;

		if (ty % 2 == 1 && tx % 2 == 0) {
			cx = margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size / 2;
			cy = margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size + hint_size / 2;
		} else if (ty % 2 == 0 && tx % 2 == 1) {
			cx = margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size + hint_size / 2;
			cy = margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size / 2;
		}

		ctx.beginPath();
		ctx.moveTo(cx - blank_x_size / 2, cy - blank_x_size / 2 + control_unit_height);
		ctx.lineTo(cx + blank_x_size / 2, cy + blank_x_size / 2 + control_unit_height);
		ctx.moveTo(cx + blank_x_size / 2, cy - blank_x_size / 2 + control_unit_height);
		ctx.lineTo(cx - blank_x_size / 2, cy + blank_x_size / 2 + control_unit_height);
		ctx.closePath();
		ctx.stroke();
	}

	Complete(CheckIfCompleted());
}

function Complete(flg) {
	if (is_completed != flg) {
		is_completed = flg;

		if (flg) {
			ctx.lineWidth = 3.0;
			ctx.strokeStyle = "#ff0000";
			ctx.strokeRect(6, 6 + control_unit_height, player_width - 12, player_height - 12);
		} else {
			ctx.lineWidth = 6.0;
			ctx.strokeStyle = "#ffffff";
			ctx.strokeRect(5, 5 + control_unit_height, player_width - 11, player_height - 11);
		}
		ctx.lineWidth = 1.0;
	}
}

function UpdateLine(tx, ty, new_state) {
	if (ty % 2 != tx % 2) {
		if (lines[ty][tx] != new_state && !is_completed) {
			history_undo.push({ y: ty, x: tx, type: lines[ty][tx] });
			history_redo = [];
			lines[ty][tx] = new_state;
			UpdatePlayerCanvas(tx, ty);
		}
	}
}

function AdjustXY(e) {
	var rect = e.target.getBoundingClientRect();

	return { x: e.clientX - rect.left - margin_x, y: e.clientY - rect.top - margin_y - control_unit_height };
}

function LocateControlButton(loc) {
	var x = loc.x + margin_x, y = loc.y + margin_y + control_unit_height;

	var locb_top = (control_unit_height - control_button_size) / 2;
	var locb_bottom = locb_top + control_button_size;

	if (locb_top <= y && y <= locb_bottom) {
		var cell_x = Math.floor((x - control_button_margin_x) / (control_button_margin_x + control_button_size));
		var rel_x = (x - control_button_margin_x) - cell_x * (control_button_margin_x + control_button_size);

		if (rel_x < control_button_size) {
			return cell_x;
		}
	}

	return -1;
}

function LocateMouse(loc) {
	var cell_x = Math.floor(loc.x / (hint_size + dot_size));
	var cell_y = Math.floor(loc.y / (hint_size + dot_size));

	var rel_x = loc.x - cell_x * (hint_size + dot_size);
	var rel_y = loc.y - cell_y * (hint_size + dot_size);

	if (
		(rel_x < dot_size + motionless_distance || hint_size + dot_size - motionless_distance < rel_x) &&
		(rel_y < dot_size + motionless_distance || hint_size + dot_size - motionless_distance < rel_y)
		) return { x: -1, y: -1 };

	if (rel_x <= dot_size) return { x: 2 * cell_x, y: 2 * cell_y + 1 };
	if (rel_y <= dot_size) return { x: 2 * cell_x + 1, y: 2 * cell_y };

	var near_x = 2 * cell_x + 1, near_y = 2 * cell_y, distance = rel_y - dot_size;

	if (distance > rel_x - dot_size) {
		near_x = 2 * cell_x;
		near_y = 2 * cell_y + 1;
		distance = rel_x - dot_size;
	}

	if (distance > hint_size + dot_size - rel_y) {
		near_x = 2 * cell_x + 1;
		near_y = 2 * cell_y + 2;
		distance = hint_size + dot_size - rel_y;
	}

	if (distance > hint_size + dot_size - rel_x) {
		near_x = 2 * cell_x + 2;
		near_y = 2 * cell_y + 1;
		distance = hint_size + dot_size - rel_x;
	}

	if (distance > allowed_distance) return { x: -1, y: -1 };
	return { x: near_x, y: near_y };
}

function DoUndo() {
	if (history_undo.length == 0) return false;

	var v = history_undo[history_undo.length - 1];
	history_redo.push({ y: v.y, x: v.x, type: lines[v.y][v.x] });
	lines[v.y][v.x] = v.type;
	history_undo.pop();
	UpdatePlayerCanvas(v.x, v.y);

	return true;
}

function DoRedo() {
	if (history_redo.length == 0) return false;

	var v = history_redo[history_redo.length - 1];
	history_undo.push({ y: v.y, x: v.x, type: lines[v.y][v.x] });
	lines[v.y][v.x] = v.type;
	history_redo.pop();
	UpdatePlayerCanvas(v.x, v.y);

	return true;
}

function PerformControlButton(button_id) {
	if (button_id == 0) {
		while (DoUndo());
	} else if (button_id == 1) {
		DoUndo();
	} else if (button_id == 2) {
		DoRedo();
	} else if (button_id == 3) {
		while (DoRedo());
	} else if (button_id == 4) {
		if (problem_id != 0) {
			--problem_id;
			InitProblem();
		}
	} else if (button_id == 5) {
		if (problem_id != problem_set.length - 1) {
			++problem_id;
			InitProblem();
		}
	}
}

function MouseDown(e) {
	var mouse_loc = AdjustXY(e);
	var loc = LocateMouse(mouse_loc);

	var button_id = LocateControlButton(mouse_loc);
	if (0 <= button_id && button_id <= 5) {
		current_button = button_id;
		DrawControlButtonBorder2(button_id, true);
	}

	if (0 <= loc.x && loc.x <= width * 2 && 0 <= loc.y && loc.y <= height * 2) {
		if (e.button == 2) next_status = (lines[loc.y][loc.x] + 2) % 3;
		else next_status = (lines[loc.y][loc.x] + 1) % 3;
		UpdateLine(loc.x, loc.y, next_status);
	}

	return false;
}

function MouseMove(e) {
	var mouse_loc = AdjustXY(e);
	var loc = LocateMouse(mouse_loc);

	var button_id = LocateControlButton(mouse_loc);
	if (current_button != -1 && button_id != current_button) {
		DrawControlButtonBorder2(current_button, false);
		current_button = -1;
	}

	if (next_status != -1 && 0 <= loc.x && loc.x <= width * 2 && 0 <= loc.y && loc.y <= height * 2) {
		UpdateLine(loc.x, loc.y, next_status);
	}
}

function MouseUp(e) {
	if (current_button != -1) {
		DrawControlButtonBorder2(current_button, false);
		PerformControlButton(current_button);
		current_button = -1;
	}

	next_status = -1;
}

function CheckIfCompleted() {
	for (var y = 0; y < height; ++y) {
		for (var x = 0; x < width; ++x) {
			var around_lines =
				(lines[y * 2 + 0][x * 2 + 1] == LINE ? 1 : 0) +
				(lines[y * 2 + 1][x * 2 + 0] == LINE ? 1 : 0) +
				(lines[y * 2 + 2][x * 2 + 1] == LINE ? 1 : 0) +
				(lines[y * 2 + 1][x * 2 + 2] == LINE ? 1 : 0);
			if (0 <= problem[y][x] && problem[y][x] <= 3 && problem[y][x] != around_lines) return false;
		}
	}

	var union_find = [];
	for (var i = 0; i < (height * 2 + 1) * (width * 2 + 1) ; ++i) union_find.push(-1);

	function root(p) {
		return union_find[p] < 0 ? p : (union_find[p] = root(union_find[p]));
	}
	function join(p, q) {
		p = root(p); q = root(q);
		if (p == q) return;
		union_find[p] += union_find[q];
		union_find[q] = p;
	}
	function id(x, y) { return y * (width * 2 + 1) + x; }

	for (var y = 0; y <= height * 2; y += 2) {
		for (var x = 0; x <= width * 2; x += 2) {
			var around_lines = 0;
			if (y > 0 && lines[y - 1][x] == LINE) {
				join(id(x, y), id(x, y - 1));
				++around_lines;
			}
			if (x > 0 && lines[y][x - 1] == LINE) {
				join(id(x, y), id(x - 1, y));
				++around_lines;
			}
			if (y < height * 2 && lines[y + 1][x] == LINE) {
				join(id(x, y), id(x, y + 1));
				++around_lines;
			}
			if (x < width * 2 && lines[y][x + 1] == LINE) {
				join(id(x, y), id(x + 1, y));
				++around_lines;
			}

			if (around_lines != 0 && around_lines != 2) return false;
		}
	}

	var line_root = -1;
	for (var y = 0; y <= height * 2; ++y) {
		for (var x = 0; x <= width * 2; ++x) {
			if (y % 2 != x % 2 && lines[y][x] == LINE) {
				if (line_root == -1) line_root = root(id(x, y));
				else if (line_root != root(id(x, y))) return false;
			}
		}
	}
	
	return true;
}
