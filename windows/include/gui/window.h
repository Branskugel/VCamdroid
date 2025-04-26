#pragma once

#include <wx/wx.h>

#include "canvas.h"
#include "net/server.h"

class Window : public wxFrame
{
public:
	struct MenuIDs
	{
		static const int QR = 100;
		static const int DEVICES = 101;
	};

	Window(Server::HostInfo hostInfo);

	Canvas* GetCanvas();

	wxChoice* GetSourceChoice();
	wxChoice* GetResolutionChoice();

	wxButton* GetRotateLeftButton();
	wxButton* GetRotateRightButton();
	wxButton* GetFlipButton();
	wxButton* GetAdjustmentsButton();
	wxButton* GetSwapButton();
private:
	Canvas* canvas;

	wxChoice* sourceChoice;
	wxChoice* resolutionChoice;

	wxButton* rotateLeftButton;
	wxButton* rotateRightButton;
	wxButton* flipButton;
	wxButton* adjustmentsButton;
	wxButton* swapButton;

	void InitializeMenu(Server::HostInfo hostinfo);
	void InitializeCanvasPanel(wxPanel* parent, wxBoxSizer* topsizer);
	void InitializeControlPanel(wxPanel* parent, wxBoxSizer* topsizer);
};