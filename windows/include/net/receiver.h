#pragma once

#include <asio.hpp>

class Receiver
{
public:

	struct FrameReceivedListener
	{
		virtual void OnFrameReceived(const unsigned char* bytes, size_t length) const = 0;
	};

	Receiver(size_t frameByteSize, const FrameReceivedListener& frameReceivedListener);
	
	void SetFrameByteSize(size_t size) const;

	void ReadSome(size_t bytes) const;
	
	asio::mutable_buffers_1 GetBuffer() const;
	void ResetBuffer() const;

private:
	bool receivingEnabled;

	const FrameReceivedListener& frameReceivedListener;

	mutable size_t frameByteSize;
	mutable size_t bytesReceived;
	mutable unsigned char* buffer;
};