#pragma once

#include "util.h"

class ClientClass;

struct Rect {
	int x, y;
	int width, height;
};
 
struct VRect {
	int	x, y, width, height;
	Rect* pnext;
};

class bf_write;
class CHLClient {
public:
	VPROXY(GetAllClasses, 8, ClientClass*, (void));
	VPROXY(CreateMove, 21, void, (int sequence_number, float input_sample_frametime, bool active), sequence_number, input_sample_frametime, active);
	VPROXY(WriteUsercmdDeltaToBuffer, 23, bool, (bf_write* buf, int from, int to, bool isnewcommand), buf, from, to, isnewcommand);
	VPROXY(View_Render, 26, void, (void* rect), rect);
};