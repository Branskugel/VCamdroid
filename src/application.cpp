#include "application.h"

#include "window.h"
#include "logger.h"

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

	mainWindow->GetSourceChoice()->Bind(wxEVT_CHOICE, [&](const wxEvent& arg) {
		int selection = mainWindow->GetSourceChoice()->GetSelection();
		server->SetUDPStreamingDevice(selection);
	});

	mainWindow->GetResolutionChoice()->Bind(wxEVT_CHOICE, [&](const wxEvent& arg) {
		/*int selection = mainWindow->GetResolutionChoice()->GetSelection();
		
		if (selection == 0)
			server->SetUDPFrameByteSize(Camera::CalculateFrameSize(640, 480));
		else if (selection == 1)
			server->SetUDPFrameByteSize(Camera::CalculateFrameSize(1280, 720));
		else if(selection == 2)
			server->SetUDPFrameByteSize(Camera::CalculateFrameSize(1920, 1080));*/
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

	mainWindow->GetSourceChoice()->SetSelection(server->GetUDPStreamingDevice());
}
