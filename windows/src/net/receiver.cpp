#include "net/receiver.h"

#include "logger.h"

Receiver::Receiver(size_t frameByteSize, const FrameReceivedListener& frameReceivedListener) 
	: frameByteSize(frameByteSize), 
	frameReceivedListener(frameReceivedListener)
{
	bytesReceived = 0;
	buffer = new unsigned char[frameByteSize];
}

asio::mutable_buffers_1 Receiver::GetBuffer() const
{
	// Read directly into the main buffer in the specified range
	// Start offset is how many bytes were received from the current
	// frame and max buffer size is how many bytes are left to be read
	return asio::buffer(buffer + bytesReceived, frameByteSize - bytesReceived);
}

void Receiver::ResetBuffer() const
{
	this->bytesReceived = 0;
}

void Receiver::SetFrameByteSize(size_t size) const
{
	frameByteSize = size;
	buffer = (unsigned char*)std::realloc(buffer, frameByteSize);

	logger << "[SERVER] Set UDP frame byte size " << size << std::endl;
}

void Receiver::ReadSome(size_t bytes) const
{
	this->bytesReceived += bytes;

	// If we received all the bytes needed for a frame
	// notify the listener and reset the receiving buffer
	if (this->bytesReceived >= this->frameByteSize)
	{
		frameReceivedListener.OnFrameReceived(buffer, this->frameByteSize);
		this->bytesReceived = 0;
	}
}
