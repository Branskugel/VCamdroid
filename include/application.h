#pragma once

#include <wx/wx.h>

#include "window.h"
#include "camera.h"

class Application : public wxApp
{
public:
	Application();
	~Application();

	virtual bool OnInit();

private:

	Window* mainWindow;

	std::unique_ptr<Camera> camera;
};