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
	
	void InitializeMenu();
	void InitializeCanvasPanel(wxPanel* parent, wxBoxSizer* topsizer);
	void InitializeControlPanel(wxPanel* parent, wxBoxSizer* topsizer);
};