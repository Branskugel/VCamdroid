#include "gui/devicesview.h"

#include <wx/listctrl.h>

DevicesView::DevicesView(std::vector<Server::DeviceInfo> devices) : wxDialog(nullptr, wxID_ANY, "Connected devices")
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	auto list = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	list->EnableSystemTheme(false);

	list->AppendColumn("#", wxLIST_FORMAT_LEFT, 20);
	list->AppendColumn("Name", wxLIST_FORMAT_LEFT, 100);
	list->AppendColumn("Address", wxLIST_FORMAT_LEFT, 100);
	list->AppendColumn("Port", wxLIST_FORMAT_LEFT);

	for (int i = 0; i < devices.size(); i++)
	{
		list->InsertItem(0, std::to_string(i + 1));
		list->SetItem(0, 1, devices[i].name);
		list->SetItem(0, 2, devices[i].address);
		list->SetItem(0, 3, std::to_string(devices[i].port));
	}

	sizer->Add(list, 0, wxEXPAND | wxALL, FromDIP(10));
	this->SetSizerAndFit(sizer);
}
