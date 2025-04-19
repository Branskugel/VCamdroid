#include "canvas.h"

#include <wx/dcbuffer.h>

Canvas::Canvas(wxWindow* parent, wxPoint position, wxSize size) 
	: wxPanel(parent, wxID_ANY, position, size), 
	size(size), 
	bitmap(size.x, size.y)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
}

void Canvas::Render(const wxImage& image)
{
	shouldDraw = true;
	this->bitmap = wxBitmap(image.Scale(size.x, size.y));
	this->Refresh();
	this->Update();
}

void Canvas::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();
	
	if (shouldDraw)
	{
		dc.DrawBitmap(bitmap, 0, 0, false);
	}
	else
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawRectangle(0, 0, size.x, size.y);
	}


}