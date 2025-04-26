#pragma once

#include <wx/wx.h>

#include "gui/window.h"
#include "net/server.h"
#include "stream.h"

#include "softcam/softcam.h"

class Application : public wxApp, public Server::ConnectionListener
{
public:
	Application();

	virtual bool OnInit();
	
	virtual void OnDeviceConnected() const override;
	virtual void OnDeviceDisconnected() const override;
private:
	Window* mainWindow;

	scCamera camera;
	std::unique_ptr<Server> server;
	std::unique_ptr<Stream> stream;
	bool backCameraActive;

	void UpdateAvailableDevices() const;

	void OnMenuEvent(wxCommandEvent& event);
	void OnWindowCloseEvent(wxCloseEvent& event);
};