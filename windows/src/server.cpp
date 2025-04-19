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
	streamingDevice = 0;
	udpBytesReceived = 0;
	// Default frame size is 640x480
	udpFrameByteSize = 640 * 480 * 3;
	buffer = new unsigned char[udpFrameByteSize];
	
	StartReceive();
}

Server::HostInfo Server::GetHostInfo()
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

	return { name, ipstr, std::to_string(port) };
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

void Server::SetUDPFrameByteSize(size_t size)
{
	//udpFrameByteSize = size;
	//buffer = (unsigned char*)std::realloc(buffer, udpFrameByteSize);

	//logger << "[SERVER] Set UDP frame byte size " << size << std::endl;
}

void Server::SetUDPStreamingDevice(int device)
{
	if (device < 0 || device >= connections.size())
		return;

	connections[device]->Send("streamstart");
	connections[device]->active = true;
	
	udpBytesReceived = 0;
	logger << "[SERVER] Set UDP stream device: " << connections[device]->socket.remote_endpoint() << std::endl;

	for (int i = 0; i < connections.size(); i++)
	{
		if(i != device)
		{
			connections[i]->active = false;
			connections[i]->Send("streamstop");
		}
	}
}

int Server::GetUDPStreamingDevice()
{
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->active)
		{
			return i;
		}
	}
	return -1;
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
	// Read directly into the main buffer in the specified range
	// Start offset is how many bytes were received from the current
	// frame and max buffer size is how many bytes are left to be read
	udpsocket.async_receive_from(
		asio::buffer(buffer + udpBytesReceived, udpFrameByteSize - udpBytesReceived),
		remote_endpoint,
		std::bind(&Server::HandleReceive, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void Server::HandleReceive(const std::error_code& ec, size_t bytesReceived)
{
	if (!ec)
	{
		this->udpBytesReceived += bytesReceived;

		// If we received all the bytes needed for a frame
		// notify the listener and reset the receiving buffer
		if (this->udpBytesReceived >= this->udpFrameByteSize)
		{
			bytesReceivedListener.OnBytesReceived(buffer, this->udpFrameByteSize);
			this->udpBytesReceived = 0;
		}

		// Mechanism to synchronize the server with the client
		// otherwise client might send the next segment of the 
		// frame before the server finished processing the current 
		// one result in loss of data
		// The client waits to read some bytes after between sending segments
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
	name(name),
	onDisconnectedListener(onDisconnectedListener)
{
	active = false;
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
	socket.send(asio::buffer(message));
}

void Server::Connection::Close()
{
	socket.close();
	onDisconnectedListener(this->shared_from_this());
}
