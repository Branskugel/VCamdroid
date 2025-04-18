#pragma once

#include <tuple>
#include <thread>
#include <asio.hpp>
#include <string>

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
class Server
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

	struct BytesReceivedListener
	{
		virtual void OnBytesReceived(const unsigned char* bytes, size_t length) const = 0;
	};

	struct DeviceInfo
	{
		std::string name;
		std::string address;
		unsigned short port;
	};

	Server(int port, const ConnectionListener& connectionListener, const BytesReceivedListener& bytesReceivedListener);

	/// <summary>
	/// Gets host device's info (name, IPv4 address and port)
	/// </summary>
	HostInfo GetHostInfo();

	void Start();
	void Close();
	void SetUDPFrameByteSize(size_t size);
	void SetUDPStreamingDevice(int device);
	int GetUDPStreamingDevice();

	std::vector<DeviceInfo> GetConnectedDevicesInfo();

private:
	/// <summary>
	/// Manages a TCP connection
	/// </summary>
	struct Connection : public std::enable_shared_from_this<Connection>
	{
		using OnDisconnectedListener = std::function<void(std::shared_ptr<Connection>)>;
		static const int BUFLEN = 128;

		OnDisconnectedListener onDisconnectedListener;
		
		std::array<char, BUFLEN> buffer;
		tcp::socket socket;
		
		bool active;
		std::string name;

		Connection(tcp::socket socket, std::string& name, OnDisconnectedListener onDisconnectedListener);

		/// <summary>
		/// Connected devices only send their name once after connecting.
		/// That is handled directly by the server when creating a new connection.
		/// This function is used only to detect when a device disconnects.
		/// </summary>
		void Read();

		void Send(std::string message);
		void Close();
	};

	int port;

	const ConnectionListener& connectionListener;
	const BytesReceivedListener& bytesReceivedListener;

	asio::io_context context;
	tcp::acceptor acceptor;
	udp::socket udpsocket;
	udp::endpoint remote_endpoint;

	std::thread thread;

	size_t udpFrameByteSize;
	size_t udpBytesReceived;
	unsigned char* buffer;

	int streamingDevice;
	std::vector<std::shared_ptr<Connection>> connections;

	void TCPDoAccept();
	void StartReceive();
	void HandleReceive(const std::error_code& ec, size_t bytesReceived);
	void OnConnectionDisconnected(std::shared_ptr<Connection> connection);
};