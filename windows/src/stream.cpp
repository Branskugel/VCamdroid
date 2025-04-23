#include "stream.h"

#include <wx/mstream.h>
#include "logger.h"

size_t Stream::CalculateFrameSize(int width, int height)
{
	return width * height * 3;
}

Stream::Stream(OnFrameReadyCallback fn) : onFrameReady(fn)
{
	closed = false;
	transforms = { 0, 0 };
	adjustments = { 0, 0 };
}

void Stream::OnFrameReceived(const unsigned char* bytes, size_t length) const
{
	if (closed)
		return;

	// Load the image directly from memory
	// https://docs.wxwidgets.org/trunk/classwx_memory_input_stream.html
	wxMemoryInputStream stream(bytes, length);
	image.LoadFile(stream, wxBITMAP_TYPE_JPEG);

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
	closed = true;
}
