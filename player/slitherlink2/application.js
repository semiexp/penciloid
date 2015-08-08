window.onload = function () {
	var cont = new SlitherlinkController;
	var board = new Board;
	var field = new Field;
	var applet = new Applet(document.getElementById("player"), cont, board);

	var problems = document.getElementsByName("problem")[0].value;
	problems = problems.split(",");
	if (problems[problems.length - 1] == "") problems.pop();

	cont.setProblems(problems);
	cont.setBoard(board);
	board.setField(field);
	cont.setProblemID(0);
}
