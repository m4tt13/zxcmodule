#include "util.h"
#include "isurface.h"

void ISurface::StartDrawing()
{
	using StartDrawingFn = void(__thiscall*)(ISurface*);
	static StartDrawingFn StartDrawing = (StartDrawingFn)findPattern("vguimatsurface.dll", "40 53 56 57 48 83 EC ?? 48 8B F9 80");

	StartDrawing(this);
}

void ISurface::FinishDrawing()
{
	using FinishDrawingFn = void(__thiscall*)(ISurface*);
	static FinishDrawingFn FinishDrawing = (FinishDrawingFn)findPattern("vguimatsurface.dll", "40 53 48 83 EC ?? 33 C9");

	FinishDrawing(this);
}