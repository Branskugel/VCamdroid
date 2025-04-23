#pragma once

#include <asio.hpp>

class Receiver
{
public:
	struct FrameReceivedListener
	{
		virtual void OnFrameReceived(const unsigned char* bytes, size_t length) const = 0;
	};

	Receiver(size_t maxFrameByteSize, size_t maxPacketSize, const FrameReceivedListener& frameReceivedListener);
	
	void ReadSome(size_t bytes) const;
	
	asio::mutable_buffers_1 GetBuffer() const;
	void Reset() const;

private:
	const FrameReceivedListener& frameReceivedListener;

	size_t maxPacketSize;
	mutable size_t maxFrameByteSize;

	// Number of total segments of a given frame. Third byte in the read buffer
	mutable size_t totalSegments;
	// Number of segments received. Second byte in the read buffer
	mutable size_t segmentsReceived;

	// The frame buffer contains all bytes that make up the image
	mutable unsigned char* frameBuffer;
	// The actual size of the frame
	mutable size_t frameSize;

	// The buffer where asio will read into
	unsigned char* readBuffer;
};