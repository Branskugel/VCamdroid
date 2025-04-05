#pragma once

#include <wx/wx.h>

#include "window.h"
#include "server.h"
#include "camera.h"

class Application : public wxApp, public Server::ConnectionListener
{
public:
	Application();
	~Application();

	virtual bool OnInit();
	
	virtual void OnDeviceConnected() const override;
	virtual void OnDeviceDisconnected() const override;

	void UpdateAvailableDevices() const;

private:

	Window* mainWindow;

	std::unique_ptr<Server> server;
	std::unique_ptr<Camera> camera;
};