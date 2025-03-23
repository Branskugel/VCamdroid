#include "camera.h"
#include "server.h"

#include <thread>

Camera::Camera() : image(300, 250)
{
	server = std::make_unique<Server>(*this);
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
	int width = 300;
	int height = 250;

	for (int i = 0; i < width * height * 3; i++)
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
