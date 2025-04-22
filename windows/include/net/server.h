#pragma once

#include <tuple>
#include <thread>
#include <asio.hpp>
#include <string>

#include "connection.h"
#include "receiver.h"

/// <summary>
/// Server implementation for the custom transmission protocol in use.
///
/// The server accepts tcp connections in order to be able to manage and
/// control connected streaming devices.
///
/// Messaging between server and clients is done over TCP while actual
/// video streaming is done over UDP. The server commands which client
/// should stream video feed.
/// </summary>
class Server : std::enable_shared_from_this<Server>, public Receiver
{
public:
	using tcp = asio::ip::tcp;
	using udp = asio::ip::udp;
	using HostInfo = std::tuple<std::string, std::string, std::string>;

	struct ConnectionListener
	{
		virtual void OnDeviceConnected() const = 0;
		virtual void OnDeviceDisconnected() const = 0;
	};

	struct DeviceInfo
	{
		std::string name;
		std::string address;
		unsigned short port;
	};

	Server(int port, const ConnectionListener& connectionListener, const Receiver::FrameReceivedListener& bytesReceivedListener);

	/// <summary>
	/// Gets host device's info (name, IPv4 address and port)
	/// </summary>
	HostInfo GetHostInfo();

	void Start();
	void Close();
	void SetUDPStreamingDevice(int device);
	int GetUDPStreamingDevice();

	std::vector<DeviceInfo> GetConnectedDevicesInfo();

private:
	int port;

	const ConnectionListener& connectionListener;

	//asio::executor_work_guard<asio::io_context::executor_type> guard;
	asio::io_context context;

	tcp::acceptor acceptor;
	udp::socket udpsocket;
	udp::endpoint remote_endpoint;

	std::thread thread;

	int streamingDevice;
	std::vector<std::shared_ptr<Connection>> connections;

	void TCPDoAccept();
	void StartReceive();
	void OnConnectionDisconnected(std::shared_ptr<Connection> connection);
};