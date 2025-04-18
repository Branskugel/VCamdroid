#include "application.h"

#include "gui/imgadjdlg.h"

Application::Application()
{
	wxInitAllImageHandlers();
	//wxImageHandler::

	mainWindow = new Window();

	stream = std::make_unique<Stream>([&](const wxImage& image) {
		mainWindow->GetCanvas()->Render(image);
	});

	server = std::make_unique<Server>(6969, *this, *stream);
	server->Start();

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

	mainWindow->GetRotateLeftButton()->Bind(wxEVT_BUTTON, [&](const wxEvent& arg) {
		stream->RotateLeft();
	});

	mainWindow->GetRotateRightButton()->Bind(wxEVT_BUTTON, [&](const wxEvent& arg) {
		stream->RotateRight();
	});

	mainWindow->GetFlipButton()->Bind(wxEVT_BUTTON, [&](const wxEvent& arg) {
		stream->Mirror();
	});

	mainWindow->GetAdjustmentsButton()->Bind(wxEVT_BUTTON, [&](const wxEvent& arg) {
		ImgAdjDlg dialog(nullptr, stream->GetAdjustments());
		
		dialog.Bind(EVT_BRIGHTNESS_CHANGED, [&](const wxCommandEvent& event) {
			stream->SetBrightnessAdjustment(event.GetInt());
		});

		dialog.Bind(EVT_SATURATION_CHANGED, [&](const wxCommandEvent& event) {
			stream->SetSaturationAdjustment(event.GetInt());
		});

		dialog.ShowModal();
	});
}

Application::~Application()
{
	server->Close();
	stream->Close();
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
