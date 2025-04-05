#include "camera.h"
#include "server.h"

#include <thread>

Camera::Camera() : image(WIDTH, HEIGHT)
{

}

void Camera::OnBytesReceived(const unsigned char* bytes, size_t length) const
{
	unsigned char* data = image.GetData();

	for (int i = 0; i < length; i++)
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
