#pragma once

#include <algorithm>
#include <cstdio>
#include "SolverConstant.h"
#include "MiniVector.hpp"
#include "UnionFind.h"
#include "GridLoopMethod.hpp"

namespace Penciloid
{

template <class T>
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
	void Init(const GridLoop<T> &src);

	inline void SetMethod(GridLoopMethod &method_t) { method = method_t; }
	inline GridLoopMethod GetMethod() { return method; }

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline int GetSegmentStyle(int y, int x) const { return segments[SegmentId(y, x)].segment_style; }
	inline int GetSegmentStyleSafe(int y, int x) const { return (IsProperCoordinate(y, x) ? segments[SegmentId(y, x)].segment_style : LOOP_BLANK); }

	inline int GetSegmentGroupSize(int y, int x) { return -segments[SegmentRoot(SegmentId(y, x))].group_root; }
	inline int GetSegmentDestination(int sy, int sx, int vy, int vx) { return segments[SegmentRoot(SegmentId(sy, sx))].GetAnotherEndSafe(SegmentId(vy, vx));; }

	inline bool IsProperCoordinate(int y, int x) const { return 0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width; }
	inline bool IsRepresentative(int y, int x) { return IsRepresentative(SegmentId(y, x)); }
	inline int SegmentId(int y, int x) const { return y * (2 * width + 1) + x; }
	inline int SegmentY(int segment) const { return segment / (2 * width + 1); }
	inline int SegmentX(int segment) const { return segment % (2 * width + 1); }

	inline int UpdateStatus(int status) { return field_status |= status; }
	inline int GetStatus() const { return field_status; }
	inline int GetProgress() const { return progress; }
	inline int GetTotalLines() const { return total_lines; }

	int DetermineLine(int y, int x);
	int DetermineBlank(int y, int x);
	int CheckAll();

	int CheckInOutRule();
	int CheckConnectability();

	void CheckVertex(int y, int x);
	inline void CheckCell(int y, int x) { if (0 <= y && y <= 2 * height && 0 <= x && x <= 2 * width) static_cast<T*>(this)->CheckCellSpecific(y, x); }

	inline void CheckCellSpecific(int y, int x) {}
	inline void CheckVertexSpecific(int y, int x) {}
	void CheckNeighborhood(int y, int x);
	inline void SegmentDetermined(int y, int x) {}

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

protected:
	void Enqueue(int y, int x) { if (IsProperCoordinate(y, x) && queue_top != -1) Enqueue(SegmentId(y, x)); }

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

	inline int VertexId(int y, int x) const { return SegmentId(y, x); }
	inline int CellId(int y, int x) const { return SegmentId(y, x); }
	inline bool IsRepresentative(int id) { return SegmentRoot(id) == id; }

	int SegmentRoot(int seg) { 
		return segments[seg].group_root < 0 ? seg : (segments[seg].group_root = SegmentRoot(segments[seg].group_root));
	}
	void Join(int seg1, int seg2);
	void JoinLines(int y, int x);

	void UpdateSegmentGroupStyle(int segment, int style);
	void CheckNeighborhoodOfSegmentGroup(int segment);
	int Succeeded();

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
	int *process_queue;

	int height, width;
	int total_lines;
	int field_status, progress;

	int queue_top, queue_end, queue_size;
	GridLoopMethod method;
};

template <class T>
GridLoop<T>::GridLoop()
{
	segments = nullptr;
	process_queue = nullptr;

	height = -1;
	width = -1;
	total_lines = 0;
	queue_top = queue_end = queue_size = -1;
	progress = 0;

	field_status = SolverStatus::NORMAL;
}

template <class T>
GridLoop<T>::~GridLoop()
{
	if (segments) delete[] segments;
	if (process_queue) delete[] process_queue;
}

template <class T>
void GridLoop<T>::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (segments) delete[] segments;
	if (process_queue) delete[] process_queue;

	segments = new LoopSegment[(height * 2 + 1) * (width * 2 + 1)];
	queue_size = height * width + (height + 1) * (width + 1) + 1;
	process_queue = new int[queue_size];
	progress = 0;
	total_lines = 0;
	field_status = SolverStatus::NORMAL;

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

	while (!IsQueueEmpty()) Dequeue();
	queue_top = -1;
}

template <class T>
void GridLoop<T>::Init(const GridLoop<T> &src)
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
	progress = src.progress;

	queue_top = -1;
}

template <class T>
int GridLoop<T>::DetermineLine(int y, int x)
{
	if (!IsProperCoordinate(y, x)) return UpdateStatus(SolverStatus::UNEXPECTED);

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

	progress += -segment.group_root;
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
		static_cast<T*>(this)->SegmentDetermined(y, x);

		DequeueAndCheckAll();
		queue_top = -1;
	} else {
		CheckNeighborhoodOfSegmentGroup(segment_id);
		JoinLines(SegmentY(adjacent_id[0]), SegmentX(adjacent_id[0]));
		JoinLines(SegmentY(adjacent_id[1]), SegmentX(adjacent_id[1]));
		static_cast<T*>(this)->SegmentDetermined(y, x);
	}

	return UpdateStatus(0);
}

template <class T>
int GridLoop<T>::DetermineBlank(int y, int x)
{
	if (!IsProperCoordinate(y, x)) return UpdateStatus(0);

	int segment_id = SegmentRoot(SegmentId(y, x));
	LoopSegment &segment = segments[segment_id];

	if (segment.segment_style == LOOP_LINE) {
		return UpdateStatus(SolverStatus::INCONSISTENT);
	} else if (segment.segment_style == LOOP_BLANK) {
		return UpdateStatus(0);
	}

	progress += -segment.group_root;
	UpdateSegmentGroupStyle(segment_id, LOOP_BLANK);

	if (queue_top == -1) {
		queue_top = queue_end = 0;
		CheckNeighborhoodOfSegmentGroup(segment_id);
		static_cast<T*>(this)->SegmentDetermined(y, x);

		DequeueAndCheckAll();
		queue_top = -1;
	} else {
		CheckNeighborhoodOfSegmentGroup(segment_id);
		static_cast<T*>(this)->SegmentDetermined(y, x);

	}

	return UpdateStatus(0);
}

template <class T>
int GridLoop<T>::Succeeded()
{
	for (int i = 0; i <= height * 2; ++i) {
		for (int j = 0; j <= width * 2; ++j) if (i % 2 != j % 2 && GetSegmentStyle(i, j) == LOOP_UNDECIDED) {
			DetermineBlank(i, j);
		}
	}
	return UpdateStatus(0);
}

template <class T>
void GridLoop<T>::DequeueAndCheckAll()
{
	while (!IsQueueEmpty()) {
		int top = Dequeue();
		int y = SegmentY(top), x = SegmentX(top);

		if (y % 2 == 0 && x % 2 == 0) CheckVertex(y, x);
		if (y % 2 == 1 && x % 2 == 1) CheckCell(y, x);

		if (GetStatus() & SolverStatus::INCONSISTENT) break;
	}
}

template <class T>
int GridLoop<T>::CheckAll()
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

template <class T>
void GridLoop<T>::CheckNeighborhood(int y, int x)
{
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

template <class T>
void GridLoop<T>::CheckNeighborhoodOfSegmentGroup(int segment)
{
	int segment_i = segment;

	do {
		static_cast<T*>(this)->CheckNeighborhood(SegmentY(segment_i), SegmentX(segment_i));

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);
}

template <class T>
void GridLoop<T>::UpdateSegmentGroupStyle(int segment, int style)
{
	int segment_i = segment;

	do {
		if (segments[segment_i].segment_style == LOOP_UNDECIDED && style == LOOP_LINE) ++total_lines;
		segments[segment_i].segment_style = style;

		segment_i = segments[segment_i].group_next;
	} while (segment_i != segment);

	if (style == LOOP_LINE && segments[segment_i].adj_vertex[0] == segments[segment_i].adj_vertex[1]) {
		if (-segments[SegmentRoot(segment_i)].group_root < total_lines) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		} else {
			Succeeded();
			UpdateStatus(SolverStatus::SUCCESS);
			return;
		}
	}
}

template <class T>
void GridLoop<T>::JoinLines(int y, int x)
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

template <class T>
void GridLoop<T>::CheckVertex(int y, int x)
{
	struct NeighborhoodData
	{
		int y, x, dest, style, weight;

		NeighborhoodData() {}
		NeighborhoodData(int y, int x, int dest, int style, int weight) : y(y), x(x), dest(dest), style(style), weight(weight) {}
	};

	int vertex_id = VertexId(y, x);
	MiniVector<NeighborhoodData, 4> line, undecided;

	static_cast<T*>(this)->CheckVertexSpecific(y, x);
	
	for (int i = 0; i < 4; ++i) {
		int y2 = y + GridConstant::GRID_DY[i], x2 = x + GridConstant::GRID_DX[i];

		if (IsProperCoordinate(y2, x2)) {
			LoopSegment &segment = segments[SegmentRoot(SegmentId(y2, x2))];
			int end = segment.GetAnotherEndSafe(vertex_id);

			if (segment.segment_style == LOOP_LINE) {
				line.push_back(NeighborhoodData(y2, x2, end, segment.segment_style, -segment.group_root));
			} else if (segment.segment_style == LOOP_UNDECIDED) {
				if (end == -1) {
					continue; // just ignore this segment
				}
				undecided.push_back(NeighborhoodData(y2, x2, end, segment.segment_style, -segment.group_root));
			}
		}
	}

	if (line.size() >= 3) {
		UpdateStatus(SolverStatus::INCONSISTENT);
		return;
	}

	if (line.size() == 2) {
		if (method.already_two_lines) {
			for (auto &nb : undecided) {
				DetermineBlank(nb.y, nb.x);
			}
		}

		Join(
			SegmentId(line[0].y, line[0].x),
			SegmentId(line[1].y, line[1].x)
			);

		return;
	}

	if (line.size() == 1) {
		int valid_y = -1, valid_x = -1;
		int line_dest = line[0].dest, line_weight = line[0].weight;

		for (auto& nb : undecided) {
			if (nb.dest == line_dest && line_weight < total_lines && method.avoid_cycle) {
				DetermineBlank(nb.y, nb.x);
			} else {
				if (valid_y == -1) {
					valid_y = nb.y;
					valid_x = nb.x;
				} else valid_y = -2;
			}
		}

		if (valid_y == -1) {
			UpdateStatus(SolverStatus::INCONSISTENT);
			return;
		} else if (valid_y != -2) {
			DetermineLine(valid_y, valid_x);
		}

		if (undecided.size() == 2 && segments[vertex_id].line_destination != -1) {
			if (segments[undecided[0].dest].line_destination == undecided[1].dest && segments[undecided[0].dest].line_weight + segments[vertex_id].line_weight < total_lines) {
				int y2 = SegmentY(segments[vertex_id].line_destination), x2 = SegmentX(segments[vertex_id].line_destination);
				int undecided_rev[2] = { -1, -1 };

				for (int i = 0; i < 4; ++i) if (IsProperCoordinate(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i])) {
					int current_segment_id = SegmentId(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i]);

					if (segments[current_segment_id].segment_style == LOOP_UNDECIDED) {
						int current_destination = segments[SegmentRoot(current_segment_id)].GetAnotherEndSafe(segments[vertex_id].line_destination);

						if (current_destination == undecided[0].dest || current_destination == undecided[1].dest) {
							if (method.hourglass_level1) DetermineBlank(y2 + GridConstant::GRID_DY[i], x2 + GridConstant::GRID_DX[i]);
						} else if (current_destination != -1) {
							if (undecided_rev[1] != -1) return;
							else if (undecided_rev[0] == -1) undecided_rev[0] = current_destination;
							else undecided_rev[1] = current_destination;
						}
					}
				}

				if (undecided_rev[1] != -1 && segments[undecided_rev[1]].line_destination == undecided_rev[0]) {
					if (segments[undecided[0].dest].line_weight + segments[undecided_rev[0]].line_weight + segments[vertex_id].line_weight >= total_lines) return;

					for (int j = 0; j < 2; ++j) {
						int y3 = SegmentY(undecided_rev[j]), x3 = SegmentX(undecided_rev[j]);
						for (int i = 0; i < 4; ++i) if (IsProperCoordinate(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i])) {
							int current_segment_id = SegmentId(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i]);

							if (segments[current_segment_id].segment_style == LOOP_UNDECIDED) {
								int current_destination = segments[SegmentRoot(current_segment_id)].GetAnotherEndSafe(undecided_rev[j]);

								if (current_destination == undecided[0].dest || current_destination == undecided[1].dest) {
									if (method.hourglass_level2) DetermineBlank(y3 + GridConstant::GRID_DY[i], x3 + GridConstant::GRID_DX[i]);
								}
							}
						}
					}
				}
			}
		}

		return;
	}

	if (line.size() == 0) {
		if (undecided.size() == 2) {
			Join(
				SegmentId(undecided[0].y, undecided[0].x),
				SegmentId(undecided[1].y, undecided[1].x)
				);
		} else if (undecided.size() == 1) {
			DetermineBlank(undecided[0].y, undecided[0].x);
		}
	}
}

template <class T>
void GridLoop<T>::Join(int seg1, int seg2)
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
			if (segb1.segment_style == LOOP_LINE) {
				Succeeded();
				UpdateStatus(SolverStatus::SUCCESS);
			}
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

template <class T>
int GridLoop<T>::CheckInOutRule()
{
	UnionFind uf(height * width * 2 + 2);
	int out_of_grid = height * width;
	
	for (int i = 0; i <= height * 2; ++i) {
		for (int j = 0; j <= width * 2; ++j) {
			if (i % 2 == 1 && j % 2 == 0) {
				int cell1 = IsProperCoordinate(i, j - 1) ? ((i / 2) * width + (j / 2 - 1)) : out_of_grid;
				int cell2 = IsProperCoordinate(i, j + 1) ? ((i / 2) * width + (j / 2)) : out_of_grid;

				if (GetSegmentStyle(i, j) == LOOP_LINE) {
					uf.Join(cell1 * 2, cell2 * 2 + 1);
					uf.Join(cell1 * 2 + 1, cell2 * 2);
				} else if (GetSegmentStyle(i, j) == LOOP_BLANK) {
					uf.Join(cell1 * 2, cell2 * 2);
					uf.Join(cell1 * 2 + 1, cell2 * 2 + 1);
				}
			} else if (i % 2 == 0 && j % 2 == 1) {
				int cell1 = IsProperCoordinate(i - 1, j) ? ((i / 2 - 1) * width + (j / 2)) : out_of_grid;
				int cell2 = IsProperCoordinate(i + 1, j) ? ((i / 2) * width + (j / 2)) : out_of_grid;

				if (GetSegmentStyle(i, j) == LOOP_LINE) {
					uf.Join(cell1 * 2, cell2 * 2 + 1);
					uf.Join(cell1 * 2 + 1, cell2 * 2);
				} else if (GetSegmentStyle(i, j) == LOOP_BLANK) {
					uf.Join(cell1 * 2, cell2 * 2);
					uf.Join(cell1 * 2 + 1, cell2 * 2 + 1);
				}
			}
		}
	}

	for (int i = 0; i <= height * width; ++i) {
		if (uf.Root(i * 2) == uf.Root(i * 2 + 1)) {
			return UpdateStatus(SolverStatus::INCONSISTENT);
		}
	}

	for (int i = 0; i <= height * 2; ++i) {
		for (int j = 0; j <= width * 2; ++j) {
			int cell1, cell2;

			if (i % 2 == 1 && j % 2 == 0) {
				cell1 = IsProperCoordinate(i, j - 1) ? ((i / 2) * width + (j / 2 - 1)) : out_of_grid;
				cell2 = IsProperCoordinate(i, j + 1) ? ((i / 2) * width + (j / 2)) : out_of_grid;
			} else if (i % 2 == 0 && j % 2 == 1) {
				cell1 = IsProperCoordinate(i - 1, j) ? ((i / 2 - 1) * width + (j / 2)) : out_of_grid;
				cell2 = IsProperCoordinate(i + 1, j) ? ((i / 2) * width + (j / 2)) : out_of_grid;
			} else continue;

			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2)) {
				DetermineBlank(i, j);
			}
			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2 + 1)) {
				DetermineLine(i, j);
			}
		}
	}

	return GetStatus();
}

template <class T>
int GridLoop<T>::CheckConnectability()
{
	UnionFind uf((2 * height + 1) * (2 * width + 1));

	for (int i = 0; i <= 2 * height; i += 2) {
		for (int j = 0; j <= 2 * width; j += 2) {
			for (int k = 0; k < 4; ++k) {
				int y = i + GridConstant::GRID_DY[k], x = j + GridConstant::GRID_DX[k];
				if (IsProperCoordinate(y, x) && GetSegmentStyle(y, x) != LOOP_BLANK) {
					uf.Join(SegmentId(i, j), SegmentId(y, x));
				}
			}
		}
	}

	int line_root = -1;
	for (int i = 0; i <= 2 * height; ++i) {
		for (int j = 0; j <= 2 * width; ++j) {
			if (i % 2 != j % 2 && GetSegmentStyle(i, j) == LOOP_LINE) {
				if (line_root == -1) line_root = uf.Root(SegmentId(i, j));
				else if (line_root != uf.Root(SegmentId(i, j))) {
					return UpdateStatus(SolverStatus::INCONSISTENT);
				}
			}
		}
	}

	return GetStatus();
}

class GridLoopSimple : public GridLoop <GridLoopSimple>
{
};

template <class T>
int GridLoopAssume(T &grid)
{
	T tmp_line, tmp_blank;
	int height = grid.GetHeight(), width = grid.GetWidth();
	bool is_updated;

	do {
		is_updated = false;
		int current_progress = grid.GetProgress();
		grid.CheckAll();
		if (current_progress < grid.GetProgress()) is_updated = true;

		for (int i = 0; i <= height * 2; ++i) {
			for (int j = 0; j <= width * 2; ++j) {
				if ((i & 1) == (j & 1)) continue;

				if (grid.GetSegmentStyle(i, j) == T::LOOP_UNDECIDED && grid.IsRepresentative(i, j)) {
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
