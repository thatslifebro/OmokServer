#pragma once
#include<string>
#include<iostream>
#include<mutex>

class PacketBufferManager
{
private:
	static PacketBufferManager* _instance;
	static std::mutex _mutex;
	PacketBufferManager() = default;
	~PacketBufferManager();
	char* _pktBuffer;
	char* _pktBufferTemp;
	int _bufferSize = 0;
	int _readPos = 0;
	int _writePos = 0;
	int _headerSize = 0;
	int _maxPacketSize = 0;
	
public:
	
	static PacketBufferManager* GetInstance();
	PacketBufferManager(const PacketBufferManager&) = delete;
	PacketBufferManager& operator=(const PacketBufferManager&) = delete;
	PacketBufferManager(PacketBufferManager&&) = delete;
	PacketBufferManager& operator=(PacketBufferManager&&) = delete;
	
	void Init(int bufferSize, int headerSize, int maxPacketSize);
	bool Write(char* data, int size);
	char* Read();
	bool NextFree();
	void BufferRelocate();
};
