#include "net/connection.h"

#include "logger.h"

Connection::Connection(const Receiver& receiver, tcp::socket socket, std::string& name, OnDisconnectedListener onDisconnectedListener)
	: receiver(receiver),
	socket(std::move(socket)),
	name(name),
	onDisconnectedListener(onDisconnectedListener)
{
	active = false;
	Read();
}

void Connection::Read()
{
	socket.async_read_some(receiver.GetBuffer(), [this](asio::error_code ec, size_t bytes) {
		if (!ec)
		{
			receiver.ReadSome(bytes);
			Read();
		}
		else
		{
			// When the connection is closed by the server this function may still be called
			// and then the socket will be invalid so wrap it in a try/catch
			// 
			// Also the close() method is called inside the try to avoid closing
			// the socket again when the server is stopped
			try {
				logger << "[CONNECTION@" << this->socket.remote_endpoint() << "] Closed with error code: " << ec << std::endl;
				Close();
			} catch(std::exception e) { }
		}
	});
}

void Connection::Send(std::string message)
{
	socket.send(asio::buffer(message));
}

void Connection::Send(unsigned char* bytes, size_t size)
{
	socket.send(asio::buffer(bytes, size));
}

void Connection::Close(bool stoppedByServer)
{
	if (stoppedByServer)
	{
		//socket.cancel();
		socket.close();
	}
	else
	{
		socket.close();
		onDisconnectedListener(this->shared_from_this());
	}
}