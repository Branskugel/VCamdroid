#include "server.h"

Server::Server(const IServerListener& listener) :
	listener(listener),
	socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13))
{
	bytesReceived = 0;
	buf = new unsigned char[640 * 480 * 3];

	StartReceive();
}

void Server::Start()
{
	try
	{
		thread = std::thread([this]() {
			context.run();
		});
	}
	catch (std::exception e)
	{

	}
}

void Server::Close()
{
	socket.close();
	context.stop();

	if (thread.joinable())
	{
		thread.join();
	}
}

void Server::StartReceive()
{
	socket.async_receive_from(
		asio::buffer(buf + bytesReceived, 640 * 480 * 3 - bytesReceived),
		remote_endpoint,
		std::bind(&Server::HandleReceive, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void Server::HandleReceive(const std::error_code& ec, size_t bytesReceived)
{
	if (!ec)
	{
		this->bytesReceived += bytesReceived;

		if (this->bytesReceived >= 921600)
		{
			std::vector<unsigned char> buffer(buf, buf + 900000);
			listener.OnBytesReceived(buffer);
			this->bytesReceived = 0;
		}

		socket.send_to(asio::buffer("done"), remote_endpoint);

		StartReceive();
	}
}
