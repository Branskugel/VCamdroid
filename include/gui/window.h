#pragma once

#include <wx/wx.h>

#include "canvas.h"

class Window : public wxFrame
{
public:
	static struct MenuIDs
	{
		static const int QR = 100;
		static const int DEVICES = 101;
	};

	Window();

	Canvas* GetCanvas();

	wxChoice* GetSourceChoice();
	wxChoice* GetResolutionChoice();

	wxButton* GetRotateLeftButton();
	wxButton* GetRotateRightButton();
	wxButton* GetFlipButton();
	wxButton* GetAdjustmentsButton();
	wxButton* GetPhotoButton();

private:
	Canvas* canvas;

	wxChoice* sourceChoice;
	wxChoice* resolutionChoice;

	wxButton* rotateLeftButton;
	wxButton* rotateRightButton;
	wxButton* flipButton;
	wxButton* adjustmentsButton;
	wxButton* photoButton;

	void InitializeMenu();
	void InitializeCanvasPanel(wxPanel* parent, wxBoxSizer* topsizer);
	void InitializeControlPanel(wxPanel* parent, wxBoxSizer* topsizer);
};