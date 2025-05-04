#pragma once

#include <wx/wx.h>

class Canvas : public wxPanel
{
public:
	Canvas(wxWindow* parent, wxPoint position, wxSize size);

	void Render(const wxImage& image);
	void SetAspectRatio(int w, int h);

private:
	wxSize size;
	wxBitmap bitmap;

	bool shouldDraw = false;
	int drawX, drawY;
	
	void OnPaint(wxPaintEvent& event);
};