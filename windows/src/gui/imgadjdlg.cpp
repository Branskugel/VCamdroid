#include "gui/imgadjdlg.h"

wxDEFINE_EVENT(EVT_BRIGHTNESS_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_SATURATION_CHANGED, wxCommandEvent);

ImgAdjDlg::ImgAdjDlg(wxWindow* parent, const Stream::Adjustments& initial) : wxDialog(parent, wxID_ANY, "Image adjustments")
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	auto sliders = new wxGridSizer(2, 2, 0, 0);
	auto buttons = new wxBoxSizer(wxHORIZONTAL);

	brightnessLabel = new wxStaticText(this, wxID_ANY, wxString::Format("Brightness: %d%%", initial.brightness));
	brightnessSlider = new wxSlider(this, wxID_ANY, initial.brightness, -100, 100);
	brightnessSlider->Bind(wxEVT_SLIDER, &ImgAdjDlg::OnBrightnessSliderChanged, this);

	saturationLabel = new wxStaticText(this, wxID_ANY, wxString::Format("Saturation: %d%%", initial.saturation));
	saturationSlider = new wxSlider(this, wxID_ANY, initial.saturation, -100, 100);
	saturationSlider->Bind(wxEVT_SLIDER, &ImgAdjDlg::OnSaturationSliderChanged, this);

	sliders->Add(brightnessLabel);
	sliders->Add(brightnessSlider);

	sliders->Add(saturationLabel);
	sliders->Add(saturationSlider);

	auto okbutton = new wxButton(this, wxID_ANY, "OK");
	okbutton->Bind(wxEVT_BUTTON, [&](const wxCommandEvent& event) { this->Close(); });
	buttons->Add(okbutton);

	auto resetbutton = new wxButton(this, wxID_ANY, "Reset");
	resetbutton->Bind(wxEVT_BUTTON, &ImgAdjDlg::ResetSliders, this);
	buttons->Add(resetbutton);

	sizer->Add(sliders, 0, wxEXPAND | wxALL, FromDIP(20));
	sizer->Add(buttons, 0, wxALIGN_CENTER | wxALL, FromDIP(10));

	this->SetSizerAndFit(sizer);
}

ImgAdjDlg::~ImgAdjDlg()
{
}

void ImgAdjDlg::OnBrightnessSliderChanged(wxCommandEvent& event)
{
	brightnessLabel->SetLabel(wxString::Format("Brightness: %d%%", event.GetInt()));
	SendEvent(EVT_BRIGHTNESS_CHANGED, event.GetInt());
}

void ImgAdjDlg::OnSaturationSliderChanged(wxCommandEvent& event)
{
	saturationLabel->SetLabel(wxString::Format("Saturation: %d%%", event.GetInt()));
	SendEvent(EVT_SATURATION_CHANGED, event.GetInt());
}

void ImgAdjDlg::ResetSliders(wxCommandEvent& event)
{
	brightnessSlider->SetValue(0);
	brightnessLabel->SetLabel("Brightness: 0%");
	SendEvent(EVT_BRIGHTNESS_CHANGED, 0);

	saturationSlider->SetValue(0);
	saturationLabel->SetLabel("Saturation: 0%");
	SendEvent(EVT_SATURATION_CHANGED, 0);
}

void ImgAdjDlg::SendEvent(int event, int value)
{
	wxCommandEvent evt(event);
	evt.SetEventObject(this);
	evt.SetInt(value);
	ProcessWindowEvent(evt);
}
