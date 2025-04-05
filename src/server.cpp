#include "server.h"

#include "logger.h"


//	========== Server implementation ==========

Server::Server(int port, const ConnectionListener& connectionListener, const BytesReceivedListener& bytesReceivedListener)
	: port(port),
	connectionListener(connectionListener),
	bytesReceivedListener(bytesReceivedListener),
	acceptor(tcp::acceptor(context, tcp::endpoint(tcp::v4(), port))),
	udpsocket(context, udp::endpoint(asio::ip::udp::v4(), port))
{
	bytesReceived = 0;
	buffer = new unsigned char[640 * 480 * 3];
	
	StartReceive();
}

std::pair<std::string, std::string> Server::GetHostInfo()
{
	std::string name = asio::ip::host_name();

	addrinfo* res;
	addrinfo hints;

	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(name.c_str(), NULL, &hints, &res);

	void* addr;
	char ipstr[INET_ADDRSTRLEN];

	while (res->ai_next != NULL)
	{
		res = res->ai_next;
	}

	addr = &((sockaddr_in*)res->ai_addr)->sin_addr;

	inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));

	return { name, ipstr };
}

void Server::Start()
{
	try
	{
		TCPDoAccept();
		thread = std::thread([this]() {
			context.run();
		});
		logger << "[SERVER] Started" << std::endl;
	}
	catch (std::exception e)
	{
		logger << "[SERVER] Start failed\n";
	}
}

void Server::Close()
{
	logger << "[SERVER] Closing...\n";

	acceptor.close();
	udpsocket.close();
	context.stop();

	if (thread.joinable())
	{
		thread.join();
	}

	logger << "[SERVER] Closed.\n";
}

std::vector<Server::DeviceInfo> Server::GetConnectedDevicesInfo()
{
	std::vector<DeviceInfo> connectionInfo;
	for (auto& connection : connections)
	{
		tcp::endpoint endpoint = connection->socket.remote_endpoint();
		DeviceInfo info = { connection->name, endpoint.address().to_string(), endpoint.port() };
		connectionInfo.push_back(info);
	}
	return connectionInfo;
}

void Server::TCPDoAccept()
{
	acceptor.async_accept([&, this](asio::error_code ec, tcp::socket socket) {
		if (!ec)
		{
			logger << "[SERVER] Device connected." << socket.remote_endpoint() << std::endl;

			// Read the only message sent by the client
			// We need to read this here because the connection listener
			// needs all the data sent by the client (trough GetConnectedDevicesInfo)
			// otherwise the connection would need to be passed to the connection
			std::array<char, 128> buf;
			size_t bytes = socket.read_some(asio::buffer(buf, 128));
			std::string name(buf.data(), bytes);

			auto conn = std::make_shared<Connection>(std::move(socket), name, std::bind(&Server::OnConnectionDisconnected, this, std::placeholders::_1));
			connections.push_back(std::move(conn));

			connectionListener.OnDeviceConnected();
		}

		TCPDoAccept();
	});
}

void Server::StartReceive()
{
	udpsocket.async_receive_from(
		asio::buffer(buffer + bytesReceived, 640 * 480 * 3 - bytesReceived),
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
			bytesReceivedListener.OnBytesReceived(buffer, 921600);
			this->bytesReceived = 0;
		}

		udpsocket.send_to(asio::buffer("done"), remote_endpoint);

		StartReceive();
	}
}

void Server::OnConnectionDisconnected(std::shared_ptr<Connection> connection)
{
	connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
	connectionListener.OnDeviceDisconnected();
}






// ============== Server::Connection Implementation ==============

Server::Connection::Connection(tcp::socket socket, std::string& name, OnDisconnectedListener onDisconnectedListener)
	: socket(std::move(socket)),
	onDisconnectedListener(onDisconnectedListener)
{
	this->name = name;
	Read();
}

void Server::Connection::Read()
{
	socket.async_read_some(asio::buffer(buffer, BUFLEN), [this](asio::error_code ec, size_t bytes) {
		if (!ec)
		{
			Read();
		}
		else
		{
			logger << "[CONNECTION@" << this->socket.remote_endpoint() << "] Closed with error code: " << ec << std::endl;
			Close();
		}
	});
}

void Server::Connection::Send(std::string message)
{
	socket.async_send(asio::buffer(message));
}

void Server::Connection::Close()
{
	socket.close();
	onDisconnectedListener(this->shared_from_this());
}
