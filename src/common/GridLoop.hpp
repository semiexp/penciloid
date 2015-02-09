#pragma once

#include <algorithm>
#include <cstdio>
#include "SolverConstant.h"

namespace Penciloid
{

template <class AuxiliarySolver>
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
	void Init(const GridLoop<AuxiliarySolver> &src);

	inline void SetAuxiliarySolver(AuxiliarySolver *auxiliary_t) { auxiliary = auxiliary_t; }
	inline AuxiliarySolver* GetAuxiliarySolver() { return auxiliary; }
	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline int GetSegmentStyle(int y, int x) const { return segments[SegmentId(y, x)].segment_style; }
	inline int GetSegmentStyleSafe(int y, int x) const { return (CheckSegmentRange(y, x) ? segments[SegmentId(y, x)].segment_style : LOOP_BLANK); }

	inline bool CheckSegmentRange(int y, int x) const { return 0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width; }
	inline bool CheckCellRange(int y, int x) const { return 0 <= y && y < height && 0 <= x && x < width; }
	inline bool CheckVertexRange(int y, int x) const { return 0 <= y && y <= height && 0 <= x && x <= width; }
	inline bool IsRepresentative(int y, int x) { return IsRepresentative(SegmentId(y, x)); }

	inline int UpdateStatus(int status) { return field_status |= status; }
	inline int GetStatus() const { return field_status; }

	int DetermineLine(int y, int x);
	int DetermineBlank(int y, int x);
	int CheckAll();

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
					fprintf(stderr, "%4d ", SegmentRoot(SegmentId(i, j)));
				} else {
					fprintf(stderr, "     ");
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
		int GetAnotherEndSafe(int vertex) {
			if (adj_vertex[0] == vertex) return adj_vertex[1];
			if (adj_vertex[1] == vertex) return adj_vertex[0];
			return -1;
		}

		union {
			struct { // As a segment
				int segment_style;
				int adj_vertex[2];
				int group_root, group_next;
			};

			struct { // As a vertex
				int line_destination;
				int line_weight;
			};
		};
		bool queue_stored;

		LoopSegment() {}
	};

	inline int SegmentId(int y, int x) const { return y * (2 * width + 1) + x; }
	inline int SegmentY(int segment) const { return segment / (2 * width + 1); }
	inline int SegmentX(int segment) const { return segment % (2 * width + 1); }
	inline int VertexId(int y, int x) const { return SegmentId(y, x); }
	inline int CellId(int y, int x) const { return SegmentId(y, x); }
	inline bool IsRepresentative(int id) { return SegmentRoot(id) == id; }

	int SegmentRoot(int seg) { 
		return segments[seg].group_root < 0 ? seg : (segments[seg].group_root = SegmentRoot(segments[seg].group_root));
	}
	void Join(int seg1, int seg2);
	void JoinLines(int y, int x);

	void CheckVertex(int y, int x);
	inline void CheckCell(int y, int x) { if (0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width) auxiliary->CheckCell(*this, y, x); }
	void UpdateSegmentGroupStyle(int segment, int style);
	void CheckNeighborhoodOfSegment(int segment);
	void CheckNeighborhoodOfSegmentGroup(int segment);

	void Enqueue(int y, int x) { if (CheckSegmentRange(y, x)) Enqueue(SegmentId(y, x)); }
	void Enqueue(int p) {
		if (segments[p].queue_stored) return;
		if (queue_end == queue_size) queue_end = 0;
		process_queue[queue_end++] = p;
		segments[p].queue_stored = true;
	}
	bool IsQueueEmpty() { return queue_top == queue_end; }
	int Dequeue() {
		if (queue_top == queue_size) queue_top = 0;
		segments[process_queue[queue_top]].queue_stored = false;
		return process_queue[queue_top++];
	}
	void DequeueAndCheckAll();

	LoopSegment *segments;
	AuxiliarySolver *auxiliary;
	int *process_queue;

	int height, width;
	int total_lines;
	int field_status;

	int queue_top, queue_end, queue_size;
};

template <class AuxiliarySolver>
GridLoop<AuxiliarySolver>::GridLoop()
{
	segments = nullptr;
	auxiliary = nullptr;
	process_queue = nullptr;

	height = -1;
	width = -1;
	total_lines = 0;
	queue_top = queue_end = queue_size = -1;

	field_status = SolverStatus::NORMAL;
}

template <class AuxiliarySolver>
GridLoop<AuxiliarySolver>::~GridLoop()
{
	if (segments) delete[] segments;
	if (process_queue) delete[] process_queue;
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (segments) delete[] segments;
	if (process_queue) delete[] process_queue;

	segments = new LoopSegment[(height * 2 + 1) * (width * 2 + 1)];
	queue_size = height * width + (height + 1) * (width + 1) + 1;
	process_queue = new int[queue_size];

	for (int i = 0; i < height * 2 + 1; ++i) {
		for (int j = 0; j < width * 2 + 1; ++j) {
			if (i % 2 == 0 && j % 2 == 0) {
				// vertex (i / 2, j / 2)
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.line_destination = -1;
				seg.line_weight = -1;
				seg.queue_stored = false;
			} else if (i % 2 == 0 && j % 2 == 1) {
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.adj_vertex[0] = VertexId(i, j - 1);
				seg.adj_vertex[1] = VertexId(i, j + 1);
				seg.group_root = -1;
				seg.group_next = SegmentId(i, j);
				seg.segment_style = LOOP_UNDECIDED;
				seg.queue_stored = false;
			} else if (i % 2 == 1 && j % 2 == 0) {
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.adj_vertex[0] = VertexId(i - 1, j);
				seg.adj_vertex[1] = VertexId(i + 1, j);
				seg.group_root = -1;
				seg.group_next = SegmentId(i, j);
				seg.segment_style = LOOP_UNDECIDED;
				seg.queue_stored = false;
			} else if (i % 2 == 1 && j % 2 == 1) {
				// cell (i / 2, j / 2)
				LoopSegment &seg = segments[SegmentId(i, j)];
				seg.queue_stored = false;
			}
		}
	}

	queue_top = queue_end = 0;
	Join(SegmentId(0, 1), SegmentId(1, 0));
	Join(SegmentId(0, width * 2 - 1), SegmentId(1, width * 2));
	Join(SegmentId(height * 2, 1), SegmentId(height * 2 - 1, 0));
	Join(SegmentId(height * 2 - 1, width * 2), SegmentId(height * 2, width * 2 - 1));

	DequeueAndCheckAll();
	queue_top = -1;
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::Init(const GridLoop<AuxiliarySolver> &src)
{
	if (segments == nullptr || height != src.height || width != src.width) {
		height = src.height;
		width = src.width;

		if (segments) delete[] segments;
		if (process_queue) delete[] process_queue;

		segments = new LoopSegment[(height * 2 + 1) * (width * 2 + 1)];
		queue_size = height * width + (height + 1) * (width + 1) + 1;
		process_queue = new int[queue_size];
	}

	memcpy(segments, src.segments, sizeof(LoopSegment) * (height * 2 + 1) * (width * 2 + 1));
	total_lines = src.total_lines;
	field_status = src.field_status;
	auxiliary = src.auxiliary;

	queue_top = -1;
}

template <class AuxiliarySolver>
int GridLoop<AuxiliarySolver>::DetermineLine(int y, int x)
{
	if (!CheckSegmentRange(y, x)) return UpdateStatus(SolverStatus::UNEXPECTED);

	int segment_id = SegmentRoot(SegmentId(y, x));
	LoopSegment &segment = segments[segment_id];

	if (segment.segment_style == LOOP_BLANK) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} else if (segment.segment_style == LOOP_LINE) {
		return UpdateStatus(0);
	}

	if (GetStatus() & SolverStatus::SUCCESS) {
		// A closed cycle is already formed
		return UpdateStatus(SolverStatus::INCONSISTENT);
	}

	segments[segment.adj_vertex[0]].line_destination = segment.adj_vertex[1];
	segments[segment.adj_vertex[1]].line_destination = segment.adj_vertex[0];
	segments[segment.adj_vertex[0]].line_weight = segments[segment.adj_vertex[1]].line_weight = -segment.group_root;

	int adjacent_id[2] = { segment.adj_vertex[0], segment.adj_vertex[1] };
	UpdateSegmentGroupStyle(segment_id, LOOP_LINE);

	if (queue_top == -1) {
		queue_top = queue_end = 0;

		CheckNeighborhoodOfSegmentGroup(segment_id);
		JoinLines(SegmentY(adjacent_id[0]), SegmentX(adjacent_id[0]));
		JoinLines(SegmentY(adjacent_id[1]), SegmentX(adjacent_id[1]));

		DequeueAndCheckAll();
		queue_top = -1;
	} else {
		CheckNeighborhoodOfSegmentGroup(segment_id);
		JoinLines(SegmentY(adjacent_id[0]), SegmentX(adjacent_id[0]));
		JoinLines(SegmentY(adjacent_id[1]), SegmentX(adjacent_id[1]));
	}

	return UpdateStatus(0);
}

template <class AuxiliarySolver>
int GridLoop<AuxiliarySolver>::DetermineBlank(int y, int x)
{
	if (!CheckSegmentRange(y, x)) return UpdateStatus(0);

	int segment_id = SegmentRoot(SegmentId(y, x));
	LoopSegment &segment = segments[segment_id];

	if (segment.segment_style == LOOP_LINE) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} else if (segment.segment_style == LOOP_BLANK) {
		return UpdateStatus(0);
	}

	UpdateSegmentGroupStyle(segment_id, LOOP_BLANK);

	if (queue_top == -1) {
		queue_top = queue_end = 0;
		CheckNeighborhoodOfSegmentGroup(segment_id);

		DequeueAndCheckAll();
		queue_top = -1;
	} else {
		CheckNeighborhoodOfSegmentGroup(segment_id);

	}

	return UpdateStatus(0);
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::DequeueAndCheckAll()
{
	while (!IsQueueEmpty()) {
		int top = Dequeue();
		int y = SegmentY(top), x = SegmentX(top);

		if (y % 2 == 0 && x % 2 == 0) CheckVertex(y, x);
		if (y % 2 == 1 && x % 2 == 1) CheckCell(y, x);
	}
}

template <class AuxiliarySolver>
int GridLoop<AuxiliarySolver>::CheckAll()
{
	bool already_processing = (queue_top != -1);

	if (!already_processing) {
		queue_top = queue_end = 0;
	}
	
	for (int i = 0; i <= height * 2; ++i) {
		for (int j = 0; j <= width * 2; ++j) {
			if (i % 2 == 0 && j % 2 == 0) Enqueue(i, j);
			if (i % 2 == 1 && j % 2 == 1) Enqueue(i, j);
		}
	}

	if (!already_processing) {
		DequeueAndCheckAll();
		queue_top = -1;
	}

	return UpdateStatus(0);
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::CheckNeighborhoodOfSegment(int segment)
{
	int y = SegmentY(segment), x = SegmentX(segment);

	// TODO : solve cells

	if (y % 2 == 1) {
		Enqueue(y - 1, x);
		Enqueue(y + 1, x);

		Enqueue(y, x - 1);
		Enqueue(y, x + 1);
		Enqueue(y - 2, x - 1);
		Enqueue(y - 2, x + 1);
		Enqueue(y + 2, x - 1);
		Enqueue(y + 2, x + 1);
	} else {
		Enqueue(y, x - 1);
		Enqueue(y, x + 1);

		Enqueue(y - 1, x);
		Enqueue(y + 1, x);
		Enqueue(y - 1, x - 2);
		Enqueue(y + 1, x - 2);
		Enqueue(y - 1, x + 2);
		Enqueue(y + 1, x + 2);
	}
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::CheckNeighborhoodOfSegmentGroup(int segment)
{
	int segment_i = segment;

	do {
		CheckNeighborhoodOfSegment(segment_i);

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::UpdateSegmentGroupStyle(int segment, int style)
{
	int segment_i = segment;

	if (style == LOOP_LINE && segments[segment_i].adj_vertex[0] == segments[segment_i].adj_vertex[1]) {
		if (-segments[SegmentRoot(segment_i)].group_root < total_lines) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		} else {
			UpdateStatus(SolverStatus::SUCCESS);
			return;
		}
	}

	do {
		if (segments[segment_i].segment_style == LOOP_UNDECIDED && style == LOOP_LINE) ++total_lines;
		segments[segment_i].segment_style = style;

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::JoinLines(int y, int x)
{
	int line_segment[2] = { -1, -1 };
	for (int i = 0; i < 4; ++i) {
		int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];

		if (GetSegmentStyleSafe(y2, x2) == LOOP_LINE) {
			if (line_segment[1] != -1) {
				UpdateStatus(SolverStatus::INCONSISTENT);
				return;
			}
			if (line_segment[0] == -1) line_segment[0] = SegmentId(y2, x2);
			else line_segment[1] = SegmentId(y2, x2);
		}
	}

	if (line_segment[1] != -1) {
		Join(line_segment[0], line_segment[1]);
	}
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::CheckVertex(int y, int x)
{
	int vertex_id = VertexId(y, x);
	int dest[4], style[4], weight[4];
	int line_count = 0, undecided_count = 0;

	auxiliary->CheckVertex(*this, y, x);

	for (int i = 0; i < 4; ++i) {
		int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];

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
				DetermineBlank(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
			} else if (style[i] == LOOP_LINE) {
				if (seg1 == -1) seg1 = SegmentId(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
				else seg2 = SegmentId(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
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
					DetermineBlank(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
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
			DetermineLine(y + GridConstant::GRID_DY[valid_id], x + GridConstant::GRID_DX[valid_id]);
		}
		if (undecided_count == 2 && segments[vertex_id].line_destination != -1) {
			int undecided[2] = { -1, -1 };

			for (int i = 0; i < 4; ++i) {
				if (style[i] == LOOP_UNDECIDED) {
					if (undecided[0] == -1) undecided[0] = dest[i];
					else undecided[1] = dest[i];
				}
			}

			if (segments[undecided[0]].line_destination == undecided[1] && segments[undecided[0]].line_weight + segments[vertex_id].line_weight < total_lines) {
				int y2 = SegmentY(segments[vertex_id].line_destination), x2 = SegmentX(segments[vertex_id].line_destination);
				int undecided_rev[2] = { -1, -1 };

				for (int i = 0; i < 4; ++i) if (CheckSegmentRange(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i])) {
					int current_segment_id = SegmentId(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i]);

					if (segments[current_segment_id].segment_style == LOOP_UNDECIDED) {
						int current_destination = segments[SegmentRoot(current_segment_id)].GetAnotherEndSafe(segments[vertex_id].line_destination);

						if (current_destination == undecided[0] || current_destination == undecided[1]) {
							DetermineBlank(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i]);
						} else if (current_destination != -1) {
							if (undecided_rev[1] != -1) return;
							else if (undecided_rev[0] == -1) undecided_rev[0] = current_destination;
							else undecided_rev[1] = current_destination;
						}
					}
				}

				if (undecided_rev[1] != -1 && segments[undecided_rev[1]].line_destination == undecided_rev[0]) {
					if (segments[undecided[0]].line_weight + segments[undecided_rev[0]].line_weight + segments[vertex_id].line_weight >= total_lines) return;

					for (int j = 0; j < 2; ++j) {
						int y3 = SegmentY(undecided_rev[j]), x3 = SegmentX(undecided_rev[j]);
						for (int i = 0; i < 4; ++i) if (CheckSegmentRange(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i])) {
							int current_segment_id = SegmentId(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i]);

							if (segments[current_segment_id].segment_style == LOOP_UNDECIDED) {
								int current_destination = segments[SegmentRoot(current_segment_id)].GetAnotherEndSafe(undecided_rev[j]);

								if (current_destination == undecided[0] || current_destination == undecided[1]) {
									DetermineBlank(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i]);
								}
							}
						}
					}
				}
			}
		}

		return;
	}

	if (line_count == 0) {
		if (undecided_count == 2) {
			int seg1 = -1, seg2 = -1;

			for (int i = 0; i < 4; ++i) {
				if (style[i] == LOOP_UNDECIDED) {
					if (seg1 == -1) seg1 = SegmentId(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
					else seg2 = SegmentId(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
				}
			}

			Join(seg1, seg2);
		} else if (undecided_count == 1) {
			for (int i = 0; i < 4; ++i) {
				if (style[i] == LOOP_UNDECIDED) {
					DetermineBlank(y + GridConstant::GRID_DY[i], x + GridConstant::GRID_DX[i]);
				}
			}
		}
	}
}

template <class AuxiliarySolver>
void GridLoop<AuxiliarySolver>::Join(int seg1, int seg2)
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

	// Update style of segments
	if (segb1.segment_style == LOOP_UNDECIDED && segb2.segment_style != LOOP_UNDECIDED) {
		UpdateSegmentGroupStyle(seg1, segb2.segment_style);
		CheckNeighborhoodOfSegmentGroup(seg1);
	} else if (segb1.segment_style == LOOP_UNDECIDED && segb2.segment_style != LOOP_UNDECIDED) {
		UpdateSegmentGroupStyle(seg2, segb1.segment_style);
		CheckNeighborhoodOfSegmentGroup(seg2);
	}

	if (segb1.adj_vertex[1] == segb2.adj_vertex[1]) {
		if ((-segb1.group_root) + (-segb2.group_root) < total_lines) {
			if (segb1.segment_style == LOOP_LINE) UpdateStatus(SolverStatus::INCONSISTENT);
			else {
				// DetermineBlank(SegmentY(seg1), SegmentX(seg1));
				UpdateSegmentGroupStyle(seg1, LOOP_BLANK);
				UpdateSegmentGroupStyle(seg2, LOOP_BLANK);
				CheckNeighborhoodOfSegmentGroup(seg1);
				CheckNeighborhoodOfSegmentGroup(seg2);
			}
		} else {
			if (segb1.segment_style == LOOP_LINE) UpdateStatus(SolverStatus::SUCCESS);
		}
	}

	// Update linked list
	std::swap(segb1.group_next, segb2.group_next);

	segments[segb1.adj_vertex[0]].line_destination = -1;

	// Update UnionFind
	if (segb1.group_root < segb2.group_root) {
		segb1.group_root += segb2.group_root;
		segb2.group_root = seg1;
		segb1.adj_vertex[0] = segb2.adj_vertex[1];

		Enqueue(segb1.adj_vertex[0]);
		Enqueue(segb1.adj_vertex[1]);

		if (segb1.segment_style == LOOP_LINE) {
			segments[segb2.adj_vertex[0]].line_destination = -1;
			segments[segb1.adj_vertex[0]].line_destination = segb1.adj_vertex[1];
			segments[segb1.adj_vertex[1]].line_destination = segb1.adj_vertex[0];
			segments[segb1.adj_vertex[0]].line_weight = segments[segb1.adj_vertex[1]].line_weight = -segb1.group_root;
		}
	} else {
		segb2.group_root += segb1.group_root;
		segb1.group_root = seg2;
		segb2.adj_vertex[0] = segb1.adj_vertex[1];

		Enqueue(segb2.adj_vertex[0]);
		Enqueue(segb2.adj_vertex[1]);

		if (segb2.segment_style == LOOP_LINE) {
			segments[segb1.adj_vertex[0]].line_destination = -1;
			segments[segb2.adj_vertex[0]].line_destination = segb2.adj_vertex[1];
			segments[segb2.adj_vertex[1]].line_destination = segb2.adj_vertex[0];
			segments[segb2.adj_vertex[0]].line_weight = segments[segb2.adj_vertex[1]].line_weight = -segb2.group_root;
		}
	}
}

class LoopNullAuxiliarySolver;

class LoopNullAuxiliarySolver
{
public:
	void CheckVertex(GridLoop<LoopNullAuxiliarySolver> &grid, int y, int x) {}
	void CheckCell(GridLoop<LoopNullAuxiliarySolver> &grid, int y, int x) {}
};

template <class AuxiliarySolver>
int GridLoopAssume(GridLoop<AuxiliarySolver> &grid)
{
	GridLoop<AuxiliarySolver> tmp_line, tmp_blank;
	int height = grid.GetHeight(), width = grid.GetWidth();
	bool is_updated;

	do {
		is_updated = false;
		grid.CheckAll();

		for (int i = 0; i <= height * 2; ++i) {
			for (int j = 0; j <= width * 2; ++j) {
				if ((i & 1) == (j & 1)) continue;

				if (grid.GetSegmentStyle(i, j) == GridLoop<AuxiliarySolver>::LOOP_UNDECIDED && grid.IsRepresentative(i, j)) {
					tmp_line.Init(grid);
					tmp_blank.Init(grid);

					tmp_line.DetermineLine(i, j);
					tmp_blank.DetermineBlank(i, j);

					if ((tmp_line.GetStatus() & SolverStatus::INCONSISTENT) && (tmp_blank.GetStatus() & SolverStatus::INCONSISTENT)) {
						return grid.UpdateStatus(SolverStatus::INCONSISTENT);
					} else if (tmp_line.GetStatus() & SolverStatus::INCONSISTENT) {
						grid.DetermineBlank(i, j);
						is_updated = true;
					} else if (tmp_blank.GetStatus() & SolverStatus::INCONSISTENT) {
						grid.DetermineLine(i, j);
						is_updated = true;
					}
				}
			}
		}
	} while (is_updated);

	return grid.GetStatus();
}

}
