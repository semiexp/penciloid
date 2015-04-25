// problem
var height, width, problem;

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

onload = Init;

function SetProblem() {
	function CharToInt(a) {
		var id = a.charCodeAt(0);
		if (48 <= id && id <= 57) return id - 48;
		if (97 <= id && id <= 122) return id - 87;
		if (65 <= id && id <= 90) return id - 29;
	}
	arg = document.getElementsByName("problem").item(0).value;
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
	canvas.onmousedown = MouseDown;
	canvas.onmousemove = MouseMove;
	canvas.onmouseup = MouseUp;
	canvas.oncontextmenu = function () { return false; }

	ctx = canvas.getContext("2d");

	is_completed = false;
	SetProblem();
	ClearLines();
	InitPlayerCanvas();
}

function InitPlayerCanvas() {
	//clear
	player_height = 2 * margin_y + height * (dot_size + hint_size) + dot_size;
	player_width = 2 * margin_x + width * (dot_size + hint_size) + dot_size;
	canvas.height = player_height;
	canvas.width = player_width;

	ctx.fillStyle = "#ffffff";
	ctx.lineWidth = 1.0;
	ctx.fillRect(0, 0, player_width, player_height);

	ctx.fillStyle = "#000000";
	ctx.strokeStyle = "#000000";
	ctx.strokeRect(1, 1, player_width - 2, player_height - 2);

	for (var y = 0; y <= height; ++y) {
		for (var x = 0; x <= width; ++x) {
			ctx.fillRect(margin_x + x * (dot_size + hint_size), margin_y + y * (dot_size + hint_size), dot_size, dot_size);
		}
	}

	ctx.font = hint_size + "px 'Consolas'";
	ctx.textAlign = "center";
	for (var y = 0; y < height; ++y) {
		for (var x = 0; x < width; ++x) {
			if (0 <= problem[y][x] && problem[y][x] <= 3) {
				ctx.fillText(problem[y][x],
					margin_x + x * (dot_size + hint_size) + dot_size + hint_size / 2,
					margin_y + y * (dot_size + hint_size) + hint_size,
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
      margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size,
      clear_width,
      hint_size
    );
	} else if (ty % 2 == 0 && tx % 2 == 1) {
		ctx.fillRect(
       margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size,
			 margin_y + Math.floor(ty / 2) * (dot_size + hint_size) - (clear_width - dot_size) / 2,
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
				margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + dot_size + (hint_size - line_height) / 2,
				line_width,
				line_height
			);
		} else if (ty % 2 == 0 && tx % 2 == 1) {
			ctx.fillRect(
				margin_x + Math.floor(tx / 2) * (dot_size + hint_size) + dot_size + (hint_size - line_height) / 2 ,
				margin_y + Math.floor(ty / 2) * (dot_size + hint_size) + (dot_size - line_width) / 2,
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
		ctx.moveTo(cx - blank_x_size / 2, cy - blank_x_size / 2);
		ctx.lineTo(cx + blank_x_size / 2, cy + blank_x_size / 2);
		ctx.moveTo(cx + blank_x_size / 2, cy - blank_x_size / 2);
		ctx.lineTo(cx - blank_x_size / 2, cy + blank_x_size / 2);
		ctx.closePath();
		ctx.stroke();
	}
}

function Complete() {
	ctx.lineWidth = 3.0;
	is_completed = true;
	ctx.strokeStyle = "#ff0000";
	ctx.strokeRect(5, 5, player_width - 10, player_height - 10);
	ctx.lineWidth = 1.0;
}

function UpdateLine(tx, ty, new_state) {
	if (ty % 2 != tx % 2) {
		if (lines[ty][tx] != new_state && !is_completed) {
			lines[ty][tx] = new_state;
			UpdatePlayerCanvas(tx, ty);
			if (CheckIfCompleted()) Complete();
		}
	}
}

function AdjustXY(e) {
	var rect = e.target.getBoundingClientRect();

	return { x: e.clientX - rect.left - margin_x, y: e.clientY - rect.top - margin_y };
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

function MouseDown(e) {
	var mouse_loc = AdjustXY(e);
	var loc = LocateMouse(mouse_loc);

	if (0 <= loc.x && loc.x <= width * 2 && 0 <= loc.y && loc.y <= height * 2) {
		if (e.button == 2) next_status = (lines[loc.y][loc.x] + 2) % 3;
		else next_status = (lines[loc.y][loc.x] + 1) % 3;
		UpdateLine(loc.x, loc.y, next_status);
	}

	return false;
}

function MouseMove(e) {
	if (next_status == -1) return;

	var mouse_loc = AdjustXY(e);
	var loc = LocateMouse(mouse_loc);

	if (0 <= loc.x && loc.x <= width * 2 && 0 <= loc.y && loc.y <= height * 2) {
		UpdateLine(loc.x, loc.y, next_status);
	}
}


function MouseUp(e) {
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
