#pragma once

#include <wx/wx.h>

#include "gui/window.h"
#include "server.h"
#include "stream.h"

class Application : public wxApp, public Server::ConnectionListener
{
public:
	Application();
	~Application();

	virtual bool OnInit();
	
	virtual void OnDeviceConnected() const override;
	virtual void OnDeviceDisconnected() const override;
private:
	Window* mainWindow;

	std::unique_ptr<Server> server;
	std::unique_ptr<Stream> stream;
	
	void UpdateAvailableDevices() const;
	void OnMenuEvent(wxCommandEvent& event);
};