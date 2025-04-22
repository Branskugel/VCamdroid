#include "net/server.h"

#include "logger.h"

Server::Server(int port, const ConnectionListener& connectionListener, const Receiver::FrameReceivedListener& frameReceivedListener)
	: Receiver(640 * 480 * 3, frameReceivedListener),
	port(port),
	connectionListener(connectionListener),
	acceptor(tcp::acceptor(context, tcp::endpoint(tcp::v4(), port))),
	udpsocket(context, udp::endpoint(asio::ip::udp::v4(), port))
{
	streamingDevice = 0;

	StartReceive();
}

Server::HostInfo Server::GetHostInfo()
{
	std::string name = asio::ip::host_name();

	// Get the active network adapter by 
	// creating a dummy UDP connection and let the OS 
	// determine what network adapter will be used
	udp::resolver resolver(context);
	udp::socket socket(context);

	socket.connect(udp::endpoint(asio::ip::address::from_string("8.8.8.8"), 80));

	// The IPv4 address needed is the address of the local endpoint assigned to the socket
	// (the network adapter actual traffic is routed trough)
	asio::ip::address local_addr = socket.local_endpoint().address();

	socket.close();

	return { name, local_addr.to_string(), std::to_string(port)};
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
	
	for (std::shared_ptr<Connection> conn : connections)
	{
		conn->Close(true);
	}

	context.stop();

	if (thread.joinable())
	{
		thread.join();
	}

	logger << "[SERVER] Closed.\n";
}

void Server::SetUDPStreamingDevice(int device)
{
	if (device < 0 || device >= connections.size())
		return;

	connections[device]->Send("streamstart");
	connections[device]->active = true;
	
	ResetBuffer();
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

			auto conn = std::make_shared<Connection>(*this, std::move(socket), name, std::bind(&Server::OnConnectionDisconnected, this, std::placeholders::_1));
			connections.push_back(std::move(conn));

			connectionListener.OnDeviceConnected();
		}

		TCPDoAccept();
	});
}

void Server::StartReceive()
{
	udpsocket.async_receive_from(GetBuffer(), remote_endpoint, [&](const std::error_code& ec, size_t bytesReceived) {
		if (!ec)
		{
			ReadSome(bytesReceived);

			// Mechanism to synchronize the server with the client
			// otherwise client might send the next segment of the 
			// frame before the server finished processing the current 
			// one result in loss of data
			// The client waits to read some bytes after between sending segments
			udpsocket.send_to(asio::buffer("done"), remote_endpoint);

			StartReceive();
		}
	});
}

void Server::OnConnectionDisconnected(std::shared_ptr<Connection> connection)
{
	connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
	connectionListener.OnDeviceDisconnected();
}
