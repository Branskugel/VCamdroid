#pragma once

#include <wx/wx.h>

#include <functional>
#include <vector>

#include "server.h"

class Camera : public Server::BytesReceivedListener
{
public:
	using OnFrameReadyCallback = std::function<void(const wxImage& image)>;

	static const int WIDTH = 640;
	static const int HEIGHT = 480;

	Camera();
	
	virtual void OnBytesReceived(const unsigned char* bytes, size_t length) const override;

	void PrepareBytes(std::vector<unsigned char>& bytes);
	void SetOnFrameReady(OnFrameReadyCallback fn);

	void Close();

private:
	std::unique_ptr<Server> server;

	wxImage image;
	OnFrameReadyCallback onFrameReady;
};
