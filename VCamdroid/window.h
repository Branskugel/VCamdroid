#pragma once

#include <wx/wx.h>

#include "canvas.h"

class Window : public wxFrame
{
public:
	Window(const char* title, int width, int height, long style);

	Canvas* GetCanvas();
private:
	Canvas* canvas;
};