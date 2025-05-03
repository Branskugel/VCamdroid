#pragma once

#include <wx/wx.h>

#include <functional>
#include <vector>

#include "net/receiver.h"

class Stream : public Receiver::FrameReceivedListener
{
public:
	using OnFrameReadyCallback = std::function<void(const wxImage& image)>;

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
		int quality;
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

	unsigned char* GetBGR(const wxImage& image);

	void Close();

private:
	bool closed;

	int bgrSize = 0;
	unsigned char* bgrData = nullptr;
	
	mutable wxImage image;
	OnFrameReadyCallback onFrameReady;

	Transforms transforms;
	Adjustments adjustments;
};
