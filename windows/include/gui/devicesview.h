#pragma once

#include <wx/wx.h>
#include "net/server.h"

class DevicesView : public wxDialog
{
public:
	DevicesView(std::vector<Server::DeviceInfo> devices);	
};