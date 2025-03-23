#include "application.h"
#include "window.h"

Application::Application()
{
	mainWindow = new Window("VCamdroid", 600, 400, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX);
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
