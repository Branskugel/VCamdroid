#include "application.h"
#include "logger.h"

Logger logger(Logger::FILE);

wxIMPLEMENT_APP(Application);

//#define DEBUG
//
//#include <asio.hpp>
//#include <iostream>
//
//#include "server.h"
//#include "logger.h"
//
//Logger logger(Logger::CONSOLE);
//
//class Listener : public Server::ConnectionListener, public Server::BytesReceivedListener
//{
//public:
//	virtual void OnClientConnected() const override { }
//	virtual void OnClientDisconnected() const override { }
//	virtual void OnBytesReceived(std::vector<unsigned char> bytes) const override { }
//};
//
//int main()
//{
//	Listener listener;
//	std::unique_ptr<Server> server = std::make_unique<Server>(6969, listener, listener);
//	server->Start();
//
//	while (1)
//	{
//
//	}
//
//	return 0;
//}