#pragma once

#include <wx/wx.h>
#include "server.h"

class DevicesView : public wxDialog
{
public:
	DevicesView(std::vector<Server::DeviceInfo> devices);	
};