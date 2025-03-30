#pragma once

#include <wx/wx.h>

#include "canvas.h"

class Window : public wxFrame
{
public:
	Window();

	Canvas* GetCanvas();

private:
	
	Canvas* canvas;
	
	wxStaticBoxSizer* InitializeControlPanel(wxPanel* parent);
};