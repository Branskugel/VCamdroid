#include "canvas.h"

#include <wx/dcbuffer.h>

Canvas::Canvas(wxWindow* parent, wxPoint position, wxSize size) : wxPanel(parent, wxID_ANY, position, size, wxSIMPLE_BORDER), size(size), bitmap(size.x, size.y)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
}

void Canvas::Render(const wxImage& image)
{
	this->bitmap = wxBitmap(image.Scale(size.x, size.y));
	this->Refresh();
	this->Update();
}

void Canvas::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	dc.DrawBitmap(bitmap, 0, 0, false);
}