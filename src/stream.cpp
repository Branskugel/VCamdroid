#include "stream.h"
#include "server.h"
#include "logger.h"

size_t Stream::CalculateFrameSize(int width, int height)
{
	return width * height * 3;
}

Stream::Stream(OnFrameReadyCallback fn) : onFrameReady(fn), image(WIDTH, HEIGHT)
{
	transforms = { 0, 0 };
	adjustments = { 0, 0 };
}

void Stream::OnBytesReceived(const unsigned char* bytes, size_t length) const
{
	unsigned char* data = image.GetData();
	std::copy(&bytes[0], &bytes[length - 1], data);

	if (onFrameReady)
	{
		onFrameReady(ApplyFrameTransformsAndAdjustments());
	}
}

void Stream::RotateLeft()
{
	transforms.rotation += 1;
	transforms.rotation %= 4;
}

void Stream::RotateRight()
{
	transforms.rotation -= 1;
	transforms.rotation %= 4;
	
	// Required because of how % operator works in c++, 4%-1=-1 when we need it to be 3
	if (transforms.rotation < 0)
		transforms.rotation = 4 + transforms.rotation;
}

void Stream::Mirror()
{
	transforms.mirror = !transforms.mirror;
}

void Stream::SetBrightnessAdjustment(int value)
{
	adjustments.brightness = value;
}

void Stream::SetSaturationAdjustment(int value)
{
	adjustments.saturation = value;
}

const Stream::Adjustments& Stream::GetAdjustments()
{
	return adjustments;
}

wxImage Stream::ApplyFrameTransformsAndAdjustments() const
{
	wxImage result = image;

	switch (transforms.rotation)
	{
	case Transforms::ROTATE_90:
		result = image.Rotate90(false);
		break;

	case Transforms::ROTATE_180:
		result = image.Rotate180();
		break;

	case Transforms::ROTATE_270:
		result = image.Rotate90();
		break;
	}
	
	if (transforms.mirror)
	{
		result = result.Mirror();
	}

	result.ChangeHSV(0, adjustments.saturation / 100.0, adjustments.brightness / 100.0);

	return result;
}

void Stream::Close()
{
}
