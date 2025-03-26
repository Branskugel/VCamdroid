#include "camera.h"
#include "server.h"

#include <thread>

Camera::Camera() : image(WIDTH, HEIGHT)
{
	server = std::make_unique<Server>(*this);
	server->Start();
}

void Camera::OnClientConnected()
{
}

void Camera::OnClientDisconnected()
{
}

void Camera::OnBytesReceived(std::vector<unsigned char> bytes) const
{
	unsigned char* data = image.GetData();

	for (int i = 0; i < bytes.size(); i++)
	{
		data[i] = bytes[i];
	}

	if (onFrameReady)
	{
		onFrameReady(image);
	}
}

void Camera::PrepareBytes(std::vector<unsigned char>& bytes)
{

}

void Camera::SetOnFrameReady(OnFrameReadyCallback fn)
{
	this->onFrameReady = fn;
}

void Camera::Close()
{
	server->Close();
}
