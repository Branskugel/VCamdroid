#pragma once

#include <wx/wx.h>
#include "stream.h"

wxDECLARE_EVENT(EVT_BRIGHTNESS_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_SATURATION_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_JPEGQUALITY_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_WB_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_EFFECT_CHANGED, wxCommandEvent);

class ImgAdjDlg : public wxDialog
{
public:
	
	ImgAdjDlg(wxWindow* parent, const Stream::Adjustments& initial);
	~ImgAdjDlg();

	const wxString whiteBalances[9] = {
		"Off",
		"Auto",
		"Incandescent",
		"Fluorescent",
		"Warm fluorescent",
		"Daylight",
		"Cloudy daylight",
		"Twilight",
		"Shade"
	};

	const wxString effects[9] = {
		"Off",
		"Mono",
		"Negative",
		"Solarize",
		"Sepia",
		"Posterize",
		"Whiteboard",
		"Blackboard",
		"Aqua"
	};

private:
	wxStaticText* brightnessLabel;
	wxSlider* brightnessSlider;

	wxStaticText* saturationLabel;
	wxSlider* saturationSlider;

	wxStaticText* qualityLabel;
	wxSlider* qualitySlider;

	wxStaticText* wbLabel;
	wxChoice* wbChoice;

	wxStaticText* effectLabel;
	wxChoice* effectChoice;

	void OnBrightnessSliderChanged(wxCommandEvent& event);
	void OnSaturationSliderChanged(wxCommandEvent& event);
	void OnQualitySliderChanged(wxCommandEvent& event);
	void OnWBChoiceChanged(wxCommandEvent& event);
	void OnEffectChoiceChanged(wxCommandEvent& event);

	void ResetSliders(wxCommandEvent& event);
	void SendEvent(int event, int value);
};