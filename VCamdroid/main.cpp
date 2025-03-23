#include "application.h"

wxIMPLEMENT_APP(Application);

/*#include <asio.hpp>
#include <iostream>

int main()
{
	try
	{
		asio::io_context context;
		asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13));

		while (true)
		{
			std::vector<unsigned char> buf(250 * 300 * 3);

			asio::ip::udp::endpoint remote_endpoint;
			int bytes = socket.receive_from(asio::buffer(buf), remote_endpoint);
			std::cout << bytes << "\n";
		}
	}
	catch (std::exception e)
	{

	}
}*/