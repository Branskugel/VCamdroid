#pragma once

#include <wx/wx.h>

class Canvas : public wxPanel
{
public:
	Canvas(wxWindow* parent, wxPoint position, wxSize size);

	void Render(const wxImage& image);

private:
	wxBitmap bitmap;
	
	void OnPaint(wxPaintEvent& event);
};