#include "application.h"

#include "window.h"

Application::Application()
{
	wxInitAllImageHandlers();
	//wxImageHandler::

	mainWindow = new Window();

	camera = std::make_unique<Camera>();
	server = std::make_unique<Server>(6969, *this, *camera);
	server->Start();

	camera->SetOnFrameReady([&](const wxImage& image) {
		mainWindow->GetCanvas()->Render(image);
	});
}

Application::~Application()
{
	server->Close();
	camera->Close();
}

bool Application::OnInit()
{
	mainWindow->Show();
	return true;
}

void Application::OnDeviceConnected() const
{
	UpdateAvailableDevices();
}

void Application::OnDeviceDisconnected() const
{
	UpdateAvailableDevices();
}

void Application::UpdateAvailableDevices() const
{
	auto deviceInfo = server->GetConnectedDevicesInfo();
	
	mainWindow->GetSourceChoice()->Clear();
	for (auto& info : deviceInfo)
	{
		mainWindow->GetSourceChoice()->Append(info.name);
	}
}
