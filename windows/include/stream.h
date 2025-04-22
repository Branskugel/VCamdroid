#pragma once

#include <wx/wx.h>

#include <functional>
#include <vector>

#include "net/receiver.h"

class Stream : public Receiver::FrameReceivedListener
{
public:
	using OnFrameReadyCallback = std::function<void(const wxImage& image)>;

	static const int WIDTH = 640;
	static const int HEIGHT = 480;

	static size_t CalculateFrameSize(int width, int height);

	struct Transforms
	{
		static const int ROTATE_90 = 1;
		static const int ROTATE_180 = 2;
		static const int ROTATE_270 = 3;

		int rotation;
		bool mirror;
	};

	struct Adjustments
	{
		int brightness;
		int saturation;
	};

	Stream(OnFrameReadyCallback fn);
	
	virtual void OnFrameReceived(const unsigned char* bytes, size_t length) const override;
	
	/// Rotates the image stream by 90 degrees counterclockwise
	void RotateLeft();

	/// Rotates the image stream by 90 degress clockwise
	void RotateRight();

	/// Mirror / flip the image on the horizontal axis
	void Mirror();

	void SetBrightnessAdjustment(int value);
	void SetSaturationAdjustment(int value);

	const Adjustments& GetAdjustments();

	wxImage ApplyFrameTransformsAndAdjustments() const;

	void Close();

private:
	bool closed;

	wxImage image;
	OnFrameReadyCallback onFrameReady;

	Transforms transforms;
	Adjustments adjustments;
};
