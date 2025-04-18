#pragma once

#include <wx/wx.h>
#include "stream.h"

wxDECLARE_EVENT(EVT_BRIGHTNESS_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_SATURATION_CHANGED, wxCommandEvent);

class ImgAdjDlg : public wxDialog
{
public:
	
	ImgAdjDlg(wxWindow* parent, const Stream::Adjustments& initial);
	~ImgAdjDlg();

private:
	wxStaticText* brightnessLabel;
	wxSlider* brightnessSlider;

	wxStaticText* saturationLabel;
	wxSlider* saturationSlider;

	void OnBrightnessSliderChanged(wxCommandEvent& event);
	void OnSaturationSliderChanged(wxCommandEvent& event);

	void ResetSliders(wxCommandEvent& event);
	void SendEvent(int event, int value);
};