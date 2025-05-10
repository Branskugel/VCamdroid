#include "gui/imgadjdlg.h"

wxDEFINE_EVENT(EVT_BRIGHTNESS_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_SATURATION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_JPEGQUALITY_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_WB_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_EFFECT_CHANGED, wxCommandEvent);

ImgAdjDlg::ImgAdjDlg(wxWindow* parent, const Stream::Adjustments& initial) : wxDialog(parent, wxID_ANY, "Image adjustments")
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	auto sliders = new wxGridSizer(5, 2, 0, 0);
	auto buttons = new wxBoxSizer(wxHORIZONTAL);

	brightnessLabel = new wxStaticText(this, wxID_ANY, wxString::Format("Brightness: %d%%", initial.brightness));
	brightnessSlider = new wxSlider(this, wxID_ANY, initial.brightness, -100, 100);
	brightnessSlider->Bind(wxEVT_SLIDER, &ImgAdjDlg::OnBrightnessSliderChanged, this);

	saturationLabel = new wxStaticText(this, wxID_ANY, wxString::Format("Saturation: %d%%", initial.saturation));
	saturationSlider = new wxSlider(this, wxID_ANY, initial.saturation, -100, 100);
	saturationSlider->Bind(wxEVT_SLIDER, &ImgAdjDlg::OnSaturationSliderChanged, this);

	qualityLabel = new wxStaticText(this, wxID_ANY, wxString::Format("JPEG Quality: %d", initial.quality));
	qualitySlider = new wxSlider(this, wxID_ANY, initial.quality, 0, 100);
	qualitySlider->Bind(wxEVT_SLIDER, &ImgAdjDlg::OnQualitySliderChanged, this);

	wbLabel = new wxStaticText(this, wxID_ANY, "White balance");
	wbChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 9, whiteBalances);
	wbChoice->SetSelection(initial.whitebalance);
	wbChoice->Bind(wxEVT_CHOICE, &ImgAdjDlg::OnWBChoiceChanged, this);

	effectLabel = new wxStaticText(this, wxID_ANY, "Filter effect");
	effectChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 9, effects);
	effectChoice->SetSelection(initial.effect);
	effectChoice->Bind(wxEVT_CHOICE, &ImgAdjDlg::OnEffectChoiceChanged, this);

	sliders->Add(brightnessLabel);
	sliders->Add(brightnessSlider);

	sliders->Add(saturationLabel);
	sliders->Add(saturationSlider);

	sliders->Add(qualityLabel);
	sliders->Add(qualitySlider);

	sliders->Add(wbLabel);
	sliders->Add(wbChoice);

	sliders->Add(effectLabel);
	sliders->Add(effectChoice);

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

void ImgAdjDlg::OnQualitySliderChanged(wxCommandEvent& event)
{
	qualityLabel->SetLabel(wxString::Format("JPEG Quality: %d", event.GetInt()));
	SendEvent(EVT_JPEGQUALITY_CHANGED, event.GetInt());
}

void ImgAdjDlg::OnWBChoiceChanged(wxCommandEvent& event)
{
	SendEvent(EVT_WB_CHANGED, event.GetSelection());
}

void ImgAdjDlg::OnEffectChoiceChanged(wxCommandEvent& event)
{
	SendEvent(EVT_EFFECT_CHANGED, event.GetSelection());
}

void ImgAdjDlg::ResetSliders(wxCommandEvent& event)
{
	brightnessSlider->SetValue(0);
	brightnessLabel->SetLabel("Brightness: 0%");
	SendEvent(EVT_BRIGHTNESS_CHANGED, 0);

	saturationSlider->SetValue(0);
	saturationLabel->SetLabel("Saturation: 0%");
	SendEvent(EVT_SATURATION_CHANGED, 0);

	qualitySlider->SetValue(80);
	qualityLabel->SetLabel("JPEG Quality: 80");
	SendEvent(EVT_JPEGQUALITY_CHANGED, 80);

	wbChoice->SetSelection(1);
	SendEvent(EVT_WB_CHANGED, 1);

	effectChoice->SetSelection(0);
	SendEvent(EVT_EFFECT_CHANGED, 0);
}

void ImgAdjDlg::SendEvent(int event, int value)
{
	wxCommandEvent evt(event);
	evt.SetEventObject(this);
	evt.SetInt(value);
	ProcessWindowEvent(evt);
}
