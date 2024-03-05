#pragma once
#include<iostream>
#include<mutex>
#include<string>
#include<print>

class PacketBufferManager
{
public:
	void Init(uint32_t bufferSize, uint32_t headerSize, uint32_t maxPacketSize);
	bool Write(char* data, uint32_t size);
	char* Read();
	bool HasEnoughBufferSpace();
	void BufferRelocate();
	void ClearBuffer();

private:
	static std::mutex mutex_;
	static char* packet_buffer_;
	static char* packet_buffer_temp_;
	static uint32_t buffer_size_;
	static uint32_t read_pos_;
	static uint32_t write_pos_;
	static uint32_t header_size_;
	static uint32_t max_packet_size_;
};
