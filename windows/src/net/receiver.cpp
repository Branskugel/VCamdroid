#include "net/receiver.h"

#include "logger.h"

Receiver::Receiver(size_t maxFrameByteSize, size_t maxPacketSize, const FrameReceivedListener& frameReceivedListener) 
	: maxFrameByteSize(maxFrameByteSize),
	maxPacketSize(maxPacketSize),
	frameReceivedListener(frameReceivedListener)
{
	readBuffer = new unsigned char[maxPacketSize];
	frameBuffer = new unsigned char[maxFrameByteSize];
	segmentsReceived = 0;
	totalSegments = 0;
	frameSize = 0;
}

asio::mutable_buffers_1 Receiver::GetBuffer() const
{
	return asio::buffer(readBuffer, maxPacketSize);
}

void Receiver::Reset() const
{
	this->totalSegments = 0;
	this->segmentsReceived = 0;
	this->frameSize = 0;
}

void Receiver::ReadSome(size_t bytes) const
{
	// TODO: Improve incoming packet handling 
	// to allow a JPEG quality over 80 reliably.
	// Current implementation might be laggy and have visual artifacts
	// when the JPEG compression quality is over 80
	
	
	// Second byte in the buffer is the current segment number
	segmentsReceived = readBuffer[1];
	if (segmentsReceived == 1)
	{
		// If this segment is the first it means a new frame is incoming 
		// so remember the total number of segments of this frame 
		totalSegments = readBuffer[2];
	}

	if (frameSize + bytes - 3 < maxFrameByteSize)
	{
		std::copy_n(readBuffer + 3, bytes - 3, frameBuffer + frameSize);
		frameSize += (bytes - 3);
	}

	logger << "SEGEMENT " << segmentsReceived << " / " << totalSegments << "\n";
	logger << bytes << " " << frameSize << " " << maxFrameByteSize << "\n";

	// If we received all the bytes needed for a frame
	// notify the listener and reset the receiving buffer
	if (segmentsReceived >= totalSegments && totalSegments > 0)
	{
		frameReceivedListener.OnFrameReceived(frameBuffer, frameSize);
		Reset();
	}
}
