#pragma once
#include<iostream>
#include<mutex>
#include<string>

class PacketBufferManager
{
public:
	
	static PacketBufferManager* GetInstance();
	PacketBufferManager(const PacketBufferManager&) = delete;
	PacketBufferManager& operator=(const PacketBufferManager&) = delete;
	PacketBufferManager(PacketBufferManager&&) = delete;
	PacketBufferManager& operator=(PacketBufferManager&&) = delete;
	
	void Init(uint32_t bufferSize, uint32_t headerSize, uint32_t maxPacketSize);
	bool Write(char* data, uint32_t size);
	char* Read();
	bool HasEnoughBufferSpace();
	void BufferRelocate();

private:
	static PacketBufferManager* instance_;
	static std::mutex mutex_;
	PacketBufferManager() = default;
	~PacketBufferManager();
	char* packet_buffer_;
	char* packet_buffer_temp_;
	uint32_t buffer_size_ = 0;
	uint32_t read_pos_ = 0;
	uint32_t write_pos_ = 0;
	uint32_t header_size_ = 0;
	uint32_t max_packet_size_ = 0;
};
