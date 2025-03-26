#pragma once

#include <wx/wx.h>

#include <functional>
#include <vector>

#include "IServerListener.h"
#include "server.h"

class Camera : public IServerListener
{
public:
	using OnFrameReadyCallback = std::function<void(const wxImage& image)>;

	static const int WIDTH = 640;
	static const int HEIGHT = 480;

	Camera();
	
	virtual void OnClientConnected() override;
	virtual void OnClientDisconnected() override;
	virtual void OnBytesReceived(std::vector<unsigned char> bytes) const override;

	void PrepareBytes(std::vector<unsigned char>& bytes);
	void SetOnFrameReady(OnFrameReadyCallback fn);

	void Close();

private:
	std::unique_ptr<Server> server;

	wxImage image;
	OnFrameReadyCallback onFrameReady;
};
