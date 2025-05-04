#include "canvas.h"

#include <wx/dcbuffer.h>

Canvas::Canvas(wxWindow* parent, wxPoint position, wxSize size) 
	: wxPanel(parent, wxID_ANY, position, size), 
	size(size), 
	bitmap(size.x, size.y)
{
	drawX = 0;
	drawY = 0;
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
}

void Canvas::Render(const wxImage& image)
{
	if (!image.IsOk())
		return;

	this->bitmap = wxBitmap(image.Scale(size.x, size.y));
	if (!bitmap.IsOk())
		return;

	shouldDraw = true;
	this->Refresh();
	this->Update();
}

void Canvas::SetAspectRatio(int w, int h)
{
	if (w == 4 && h == 3) 
	{
		size = wxSize(400, 300);
		bitmap = wxBitmap(400, 300);
		drawX = 0;
		drawY = 0;
	}
	else if (w == 16 && h == 9) 
	{
		size = wxSize(400, 225);
		bitmap = wxBitmap(400, 225);
		drawX = 0;
		drawY = 37;
	}
	else if (h == 4 && w == 3)
	{
		size = wxSize(225, 300);
		bitmap = wxBitmap(225, 300);
		drawX = 87;
		drawY = 0;
	}
	else if (h == 16 && w == 9)
	{
		size = wxSize(169, 300);
		bitmap = wxBitmap(169, 300);
		drawX = 115;
		drawY = 0;
	}
}

void Canvas::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();
	
	if (shouldDraw)
	{
		dc.DrawBitmap(bitmap, drawX, drawY, false);
	}
	else
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(0, 0, size.x, size.y);
	}
}