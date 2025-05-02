#include "application.h"

#include "gui/imgadjdlg.h"
#include "gui/devicesview.h"
#include "gui/qrconview.h"
#include "adb.h"
#include "settings.h"

#include <qrcodegen.hpp>

Application::Application()
{
	Settings::load();
	wxInitAllImageHandlers();
	//wxImageHandler::
	
	// Create a camera handle to access the DirechShow Virtual Camera filter
	camera = scCreateCamera(640, 480, 30);

	backCameraActive = true;
	stream = std::make_unique<Stream>([&](const wxImage& image) {
		mainWindow->GetCanvas()->Render(image);

		// Send the current image frame to the DirechShow Virtual Camera filter
		scSendFrame(camera, image.GetData());
	});

	server = std::make_unique<Server>(6969, *this, *stream);
	server->Start();
	adb::start(6969);

	mainWindow = new Window(server->GetHostInfo());

	mainWindow->Bind(wxEVT_CLOSE_WINDOW, &Application::OnWindowCloseEvent, this);
	mainWindow->Bind(wxEVT_MENU, &Application::OnMenuEvent, this);

	mainWindow->GetSourceChoice()->Bind(wxEVT_CHOICE, [&](const wxEvent& arg) {
		int selection = mainWindow->GetSourceChoice()->GetSelection();
		server->SetStreamingDevice(selection);
	});

	mainWindow->GetResolutionChoice()->Bind(wxEVT_CHOICE, [&](const wxEvent& arg) {
		int selection = mainWindow->GetResolutionChoice()->GetSelection();
		
		if (selection == 0)
		{
			mainWindow->GetCanvas()->SetAspectRatio(4, 3);
			server->SetStreamResolution(640, 480);
		}
		else if (selection == 1)
		{
			mainWindow->GetCanvas()->SetAspectRatio(16, 9);
			if (selection == 1)
				server->SetStreamResolution(1280, 720);
			else if (selection == 2)
				server->SetStreamResolution(1920, 1080);
		}
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

	mainWindow->GetSwapButton()->Bind(wxEVT_BUTTON, [&](const wxEvent& arg) {
		backCameraActive = !backCameraActive;
		server->SetStreamingCamera(backCameraActive);
	});
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

	mainWindow->GetSourceChoice()->SetSelection(server->GetStreamingDevice());
}

void Application::OnMenuEvent(wxCommandEvent& event)
{
	switch (event.GetId())
	{
		case Window::MenuIDs::DEVICES:
		{
			DevicesView devlistview(server->GetConnectedDevicesInfo());
			devlistview.ShowModal();
			break;
		}

		case Window::MenuIDs::QR:
		{
			auto info = server->GetHostInfo();
			QrconView qrview(std::get<0>(info), std::get<1>(info), std::get<2>(info), wxSize(150, 150));
			qrview.ShowModal();
			
			break;
		}
		
		case Window::MenuIDs::HIDE2TRAY:
		{
			Settings::set("MINIMIZE_TASKBAR", event.IsChecked() ? 1 : 0);
			break;
		}
	}
}

void Application::OnWindowCloseEvent(wxCloseEvent& event)
{
	stream->Close();
	server->Close();
	adb::kill(6969);
	Settings::save();
	mainWindow->Destroy();
}
