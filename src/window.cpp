#include "window.h"

#include <iostream>
#include <thread>
#include <wx/dc.h>
#include <wx/artprov.h>

Window::Window()
	: wxFrame(nullptr, wxID_ANY, "VCamdroid", wxDefaultPosition, wxSize(600, 500), wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX & ~wxRESIZE_BORDER)
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

	canvas = new Canvas(panel, wxDefaultPosition, wxSize(320, 240));
	topsizer->Add(canvas, 0, wxALL | wxALIGN_CENTER_HORIZONTAL);

	auto controlPanel = InitializeControlPanel(panel);
	topsizer->Add(controlPanel);

	panel->SetSizerAndFit(topsizer);
}

wxStaticBoxSizer* Window::InitializeControlPanel(wxPanel* parent)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxHORIZONTAL, parent, "Controls");

	wxButton* button1 = new wxButton(parent, wxID_ANY, "ZOOM IN (+)");
	sizer->Add(button1);

	wxButton* button2 = new wxButton(parent, wxID_ANY, "ZOOM OUT (-)");
	sizer->Add(button2);

	wxButton* button3 = new wxButton(parent, wxID_ANY, "WB");
	sizer->Add(button3);

	wxButton* button4 = new wxButton(parent, wxID_ANY, "CONTRAST");
	sizer->Add(button4);

	return sizer;
}

Canvas* Window::GetCanvas()
{
	return canvas;
}