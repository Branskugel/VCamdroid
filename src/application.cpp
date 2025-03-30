#include "application.h"

#include "window.h"

Application::Application()
{
	wxInitAllImageHandlers();
	//wxImageHandler::

	mainWindow = new Window();
	camera = std::make_unique<Camera>();

	camera->SetOnFrameReady([&](const wxImage& image) {
		mainWindow->GetCanvas()->Render(image);
	});
}

Application::~Application()
{
	camera->Close();
}

bool Application::OnInit()
{
	mainWindow->Show();
	return true;
}
