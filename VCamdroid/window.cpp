#include "window.h"

#include <iostream>
#include <thread>
#include <wx/dc.h>

Window::Window(const char* title, int width, int height, long style)
	: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(width, height), style)
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

	canvas = new Canvas(panel, wxDefaultPosition, wxSize(400, 300));

	topsizer->Add(canvas);
	panel->SetSizerAndFit(topsizer);
}

Canvas* Window::GetCanvas()
{
	return canvas;
}
