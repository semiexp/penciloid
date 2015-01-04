#pragma once

#include <algorithm>
#include <cstdio>
#include "SolverConstant.h"

namespace Penciloid
{

template <class Vertex, class Cell>
class GridLoop
{
public:
	enum {
		LOOP_UNDECIDED = 0,
		LOOP_LINE = 1,
		LOOP_BLANK = 2
	};

	GridLoop();
	~GridLoop();

	void Init(int height_t, int width_t);

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline int GetSegmentStyle(int y, int x) { return segments[SegmentRoot(SegmentId(y, x))].segment_style; }

	int DetermineLine(int y, int x);
	int DetermineBlank(int y, int x);

	void Debug() {
		for (int i = 0; i <= height * 2; ++i) {
			for (int j = 0; j <= width * 2; ++j) {
				if (i % 2 == 0 && j % 2 == 0) {
					fprintf(stderr, "+");
				} else if (i % 2 == 0 && j % 2 == 1) {
					int style = GetSegmentStyle(i, j);
					if (style == LOOP_UNDECIDED) fprintf(stderr, "   ");
					if (style == LOOP_LINE) fprintf(stderr, "---");
					if (style == LOOP_BLANK) fprintf(stderr, " X ");
				} else if (i % 2 == 1 && j % 2 == 0) {
					int style = GetSegmentStyle(i, j);
					if (style == LOOP_UNDECIDED) fprintf(stderr, " ");
					if (style == LOOP_LINE) fprintf(stderr, "|");
					if (style == LOOP_BLANK) fprintf(stderr, "X");
				} else if (i % 2 == 1 && j % 2 == 1) {
					fprintf(stderr, "   ");
				}
			}
			fprintf(stderr, "\n");
		}
	}
	void Debug2() {
		for (int i = 0; i <= height * 2; ++i) {
			for (int j = 0; j <= width * 2; ++j) {
				if (i % 2 != j % 2) {
					fprintf(stderr, "%3d ", SegmentRoot(SegmentId(i, j)));
				} else {
					fprintf(stderr, "    ");
				}
			}
			fprintf(stderr, "\n");
		}
	}

private:
	struct LoopSegment
	{
		// NOTE: unsafe method (it assumes that vertex equals to adj_vertex[0] or adj_vertex[1])
		int GetAnotherEnd(int vertex) { return adj_vertex[0] + adj_vertex[1] - vertex; }

		int segment_style;
		int adj_vertex[2];
		int group_root, group_next;

		LoopSegment() {} 
	};

	inline int UpdateStatus(int status) { return field_status |= status; }
	inline bool CheckSegmentRange(int y, int x) const { return 0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width; }
	inline int SegmentId(int y, int x) const { return y * (2 * width + 1) + x; }
	inline int SegmentY(int segment) const { return segment / (2 * width + 1); }
	inline int SegmentX(int segment) const { return segment % (2 * width + 1); }
	inline int VertexId(int y, int x) const { return y * (width + 1) + x; }
	inline int CellId(int y, int x) const { return y * width + x; }

	int SegmentRoot(int seg) { 
		return segments[seg].group_root < 0 ? seg : (segments[seg].group_root = SegmentRoot(segments[seg].group_root));
	}
	void Join(int seg1, int seg2);

	void CheckVertex(int y, int x);
	void UpdateSegmentGroupStyle(int segment, int style);
	void CheckNeighborHoodOfSegment(int segment);
	void CheckNeighborHoodOfSegmentGroup(int segment);

	LoopSegment *segments;
	Vertex *verteces;
	Cell *cells;

	int height, width;
	int total_lines;
	int field_status;
};

template <class Vertex, class Cell>
GridLoop<Vertex, Cell>::GridLoop()
{
	segments = nullptr;
	verteces = nullptr;
	cells = nullptr;

	height = -1;
	width = -1;
	total_lines = 0;

	field_status = SolverStatus::NORMAL;
}

template <class Vertex, class Cell>
GridLoop<Vertex, Cell>::~GridLoop()
{
	if (segments) delete[] segments;
	if (verteces) delete[] verteces;
	if (cells) delete[] cells;
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (segments) delete[] segments;
	if (verteces) delete[] verteces;
	if (cells) delete[] cells;

	segments = new LoopSegment[(height * 2 + 1) * (width * 2 + 1)];
	verteces = new Vertex[(height + 1) * (width + 1)];
	cells = new Cell[height * width];

	for (int i = 0; i < height * 2 + 1; ++i) {
		for (int j = 0; j < width * 2 + 1; ++j) {
			if (i % 2 == 0 && j % 2 == 0) {
				// vertex (i / 2, j / 2)
			} else if (i % 2 == 0 && j % 2 == 1) {
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.adj_vertex[0] = VertexId(i / 2, j / 2);
				seg.adj_vertex[1] = VertexId(i / 2, j / 2 + 1);
				seg.group_root = -1;
				seg.group_next = SegmentId(i, j);
				seg.segment_style = LOOP_UNDECIDED;
			} else if (i % 2 == 1 && j % 2 == 0) {
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.adj_vertex[0] = VertexId(i / 2, j / 2);
				seg.adj_vertex[1] = VertexId(i / 2 + 1, j / 2);
				seg.group_root = -1;
				seg.group_next = SegmentId(i, j);
				seg.segment_style = LOOP_UNDECIDED;
			} else if (i % 2 == 1 && j % 2 == 1) {
				// cell (i / 2, j / 2)
			}
		}
	}

	Join(SegmentId(0, 1), SegmentId(1, 0));
	Join(SegmentId(0, width * 2 - 1), SegmentId(1, width * 2));
	Join(SegmentId(height * 2, 1), SegmentId(height * 2 - 1, 0));
	Join(SegmentId(height * 2 - 1, width * 2), SegmentId(height * 2, width * 2 - 1));
}

template <class Vertex, class Cell>
int GridLoop<Vertex, Cell>::DetermineLine(int y, int x)
{
	int segment_id = SegmentRoot(SegmentId(y, x));
	LoopSegment &segment = segments[segment_id];

	if (segment.segment_style == LOOP_BLANK) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} else if (segment.segment_style == LOOP_LINE) {
		return UpdateStatus(0);
	}

	UpdateSegmentGroupStyle(segment_id, LOOP_LINE);
	CheckNeighborHoodOfSegmentGroup(segment_id);

	return UpdateStatus(0);
}

template <class Vertex, class Cell>
int GridLoop<Vertex, Cell>::DetermineBlank(int y, int x)
{
	int segment_id = SegmentRoot(SegmentId(y, x));
	LoopSegment &segment = segments[segment_id];

	if (segment.segment_style == LOOP_LINE) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} else if (segment.segment_style == LOOP_BLANK) {
		return UpdateStatus(0);
	}

	UpdateSegmentGroupStyle(segment_id, LOOP_BLANK);
	CheckNeighborHoodOfSegmentGroup(segment_id);

	return UpdateStatus(0);
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::CheckNeighborHoodOfSegment(int segment)
{
	int y = SegmentY(segment), x = SegmentX(segment);

	// TODO : solve cells

	if (y % 2 == 1) {
		CheckVertex(y / 2, x / 2);
		CheckVertex(y / 2 + 1, x / 2);
	} else {
		CheckVertex(y / 2, x / 2);
		CheckVertex(y / 2, x / 2 + 1);
	}
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::CheckNeighborHoodOfSegmentGroup(int segment)
{
	int segment_i = segment;

	do {
		CheckNeighborHoodOfSegment(segment_i);

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::UpdateSegmentGroupStyle(int segment, int style)
{
	int segment_i = segment;

	do {
		if (segments[segment_i].segment_style == LOOP_UNDECIDED && style == LOOP_LINE) ++total_lines;
		segments[segment_i].segment_style = style;

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::CheckVertex(int y, int x)
{
	int vertex_id = VertexId(y, x);
	int dest[4], style[4], weight[4];
	int line_count = 0, undecided_count = 0;

	verteces[vertex_id](*this);

	for (int i = 0; i < 4; ++i) {
		int y2 = 2 * y + GridConstant::GRID_DY[i], x2 = 2 * x + GridConstant::GRID_DX[i];

		if (!CheckSegmentRange(y2, x2)) {
			dest[i] = -1;
			style[i] = LOOP_BLANK;
			weight[i] = 0;
		} else {
			LoopSegment &segment = segments[SegmentRoot(SegmentId(y2, x2))];
			dest[i] = segment.GetAnotherEnd(vertex_id);
			style[i] = segment.segment_style;
			weight[i] = -segment.group_root;

			if (style[i] == LOOP_LINE) ++line_count;
			else if (style[i] == LOOP_UNDECIDED) ++undecided_count;
		}
	}

	if (line_count >= 3) {
		UpdateStatus(SolverStatus::INCONSISTENT);
		return;
	}

	if (line_count == 2) {
		int seg1 = -1, seg2 = -1;

		for (int i = 0; i < 4; ++i) {
			if (style[i] == LOOP_UNDECIDED) {
				DetermineBlank(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
			} else if (style[i] == LOOP_LINE) {
				if (seg1 == -1) seg1 = SegmentId(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
				else seg2 = SegmentId(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
			}
		}

		Join(seg1, seg2);

		return;
	}

	if (line_count == 1) {
		int valid_id = -1;
		int line_dest, line_weight;

		for (int i = 0; i < 4; ++i) {
			if (style[i] == LOOP_LINE) {
				line_dest = dest[i];
				line_weight = weight[i];
			}
		}

		for (int i = 0; i < 4; ++i) {
			if (style[i] == LOOP_UNDECIDED) {
				if (dest[i] == line_dest && line_weight < total_lines) {
					DetermineBlank(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
				} else {
					if (valid_id == -1) valid_id = i;
					else valid_id = -2;
				}
			}
		}

		if (valid_id == -1) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		} else if (valid_id != -2) {
			DetermineLine(2 * y + GridConstant::GRID_DY[valid_id], 2 * x + GridConstant::GRID_DX[valid_id]);
		}

		return;
	}

	if (line_count == 0) {
		if (undecided_count == 2) {
			int seg1 = -1, seg2 = -1;

			for (int i = 0; i < 4; ++i) {
				if (style[i] == LOOP_UNDECIDED) {
					if (seg1 == -1) seg1 = SegmentId(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
					else seg2 = SegmentId(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
				}
			}

			Join(seg1, seg2);
		} else if (undecided_count == 1) {
			for (int i = 0; i < 4; ++i) {
				if (style[i] == LOOP_UNDECIDED) {
					DetermineBlank(2 * y + GridConstant::GRID_DY[i], 2 * x + GridConstant::GRID_DX[i]);
				}
			}
		}
	}
}

template <class Vertex, class Cell>
void GridLoop<Vertex, Cell>::Join(int seg1, int seg2)
{
	seg1 = SegmentRoot(seg1);
	seg2 = SegmentRoot(seg2);

	if (seg1 == seg2) return;

	LoopSegment &segb1 = segments[seg1], &segb2 = segments[seg2];

	if (segb1.segment_style != segb2.segment_style) {
		if (segb1.segment_style != LOOP_UNDECIDED && segb2.segment_style != LOOP_UNDECIDED) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		}
	}

	// Here assure segb1.adj_vertex[0] == segb2.adj_vertex[0]
	if (segb1.adj_vertex[0] == segb2.adj_vertex[0]) {
	} else if (segb1.adj_vertex[0] == segb2.adj_vertex[1]) {
		std::swap(segb2.adj_vertex[0], segb2.adj_vertex[1]);
	} else if (segb1.adj_vertex[1] == segb2.adj_vertex[0]) {
		std::swap(segb1.adj_vertex[0], segb1.adj_vertex[1]);
	} else if (segb1.adj_vertex[1] == segb2.adj_vertex[1]) {
		std::swap(segb1.adj_vertex[0], segb1.adj_vertex[1]);
		std::swap(segb2.adj_vertex[0], segb2.adj_vertex[1]);
	} else {
		UpdateStatus(SolverStatus::UNEXPECTED);
		return;
	}

	// Update linked list
	std::swap(segb1.group_next, segb2.group_next);

	// Update UnionFind
	if (segb1.group_root < segb2.group_root) {
		segb1.group_root += segb2.group_root;
		segb2.group_root = seg1;
		segb1.adj_vertex[0] = segb2.adj_vertex[1];
	} else {
		segb2.group_root += segb1.group_root;
		segb1.group_root = seg2;
		segb2.adj_vertex[0] = segb1.adj_vertex[1];
	}

	// TODO: check correctness of codes below

	// Update style of segments
	if (segb1.segment_style == LOOP_UNDECIDED && segb2.segment_style != LOOP_UNDECIDED) {
		UpdateSegmentGroupStyle(seg1, segb2.segment_style);
	} else if (segb1.segment_style == LOOP_UNDECIDED && segb2.segment_style != LOOP_UNDECIDED) {
		UpdateSegmentGroupStyle(seg2, segb1.segment_style);
	}

	// TODO: potentially too slow
	CheckNeighborHoodOfSegmentGroup(seg1);
}

class LoopNullVertex;
class LoopNullCell;

class LoopNullVertex
{
public:
	inline void operator()(GridLoop<LoopNullVertex, LoopNullCell> &grid) {}
};

class LoopNullCell
{
public:
	inline void operator()(GridLoop<LoopNullVertex, LoopNullCell> &grid) {}
};

}
