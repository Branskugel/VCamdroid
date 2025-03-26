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

		std::vector<unsigned char> buf(800 * 600 * 3);
		size_t bytes = 0;
		while (true)
		{
			std::vector<unsigned char> buf2(65472);
			asio::ip::udp::endpoint remote_endpoint;

			bytes += socket.receive_from(asio::buffer(buf2), remote_endpoint);

			std::cout << "Recevied " << bytes << " bytes\n";

			buf.insert(buf.end(), buf2.begin(), buf2.end());

			if (bytes >= 921000)
			{
				std::cout << "Received whole frame\n";
				bytes = 0;
				buf.clear();
			}

		}
	}
	catch (std::exception e)
	{

	}
}*/