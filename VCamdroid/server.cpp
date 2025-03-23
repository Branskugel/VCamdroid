#include "server.h"

#include <asio.hpp>

Server::Server(const IServerListener& listener)
{
	tShouldClose = false;
	thread = std::thread([&]() {

		try
		{
			asio::io_context context;
			asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13));

			while (!tShouldClose)
			{
				std::vector<unsigned char> buf(250 * 300 * 3);
				
				asio::ip::udp::endpoint remote_endpoint;
				socket.receive_from(asio::buffer(buf), remote_endpoint);

				listener.OnBytesReceived(buf);
			}
		}
		catch (std::exception e)
		{

		}
	});


	/*tShouldClose = false;
	thread = std::thread([&]() {

		int it = 0;
		std::vector<unsigned char> bytes;

		while (!tShouldClose && it < 100000)
		{
			bytes.clear();
			for (int i = 0; i < 300 * 250; i++)
			{
				bytes.push_back(rand() % 255);
				bytes.push_back(rand() % 255);
				bytes.push_back(rand() % 255);
			}

			listener.OnBytesReceived(bytes);

			it++;
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
	});
	*/
}

void Server::Close()
{
	tShouldClose = true;
	if (thread.joinable())
	{
		thread.join();
	}
}
