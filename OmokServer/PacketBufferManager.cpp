#include "PacketBufferManager.h"

PacketBufferManager* PacketBufferManager::_instance = nullptr;
std::mutex PacketBufferManager::_mutex;

PacketBufferManager* PacketBufferManager::GetInstance() {
	if(_instance == nullptr)
		_instance = new PacketBufferManager();
	return _instance;
}

PacketBufferManager::~PacketBufferManager() {
	delete[] _pktBuffer;
	delete[] _pktBufferTemp;
};

void PacketBufferManager::Init(int bufferSize, int headerSize, int maxPacketSize)
{
	_pktBuffer = new char[bufferSize];
	_pktBufferTemp = new char[bufferSize];
	_bufferSize = bufferSize;
	_headerSize = headerSize;
	_maxPacketSize = maxPacketSize;
}

bool PacketBufferManager::Write(char* data, int size)
{
	std::lock_guard<std::mutex> lock(_mutex);
	
	auto remainBufferSize = _bufferSize - _writePos;
	if (remainBufferSize < size) {
		std::cout << "Buffer is full" << std::endl;
		return false;
	}
	std::copy(data, data + size, _pktBuffer + _writePos);
	_writePos += size;

	if (NextFree() == false) {
		BufferRelocate();
	}
	return true;
}

char* PacketBufferManager::Read() {
	std::lock_guard<std::mutex> lock(_mutex);
	auto readableSize = _writePos - _readPos;
	if (readableSize < _headerSize) {
		return nullptr;
	}

	auto pktSize = ((_pktBuffer + _readPos)[0] << 8) | ((_pktBuffer + _readPos)[1]);
	std::cout<< "pktSize : " << pktSize << std::endl;

	if (readableSize < pktSize) {
		return nullptr;
	}

	auto packetData = new char[pktSize];
	std::copy(_pktBuffer + _readPos, _pktBuffer + _readPos + pktSize, packetData);
	_readPos += pktSize;
	return packetData;
}

bool PacketBufferManager::NextFree()
{
	return (_bufferSize - _readPos) >= _maxPacketSize;
}

void PacketBufferManager::BufferRelocate()
{
	std::copy(_pktBuffer + _readPos, _pktBuffer + _writePos, _pktBufferTemp);
	auto leftReadableSize = _writePos - _readPos;
	std::copy(_pktBufferTemp, _pktBufferTemp + leftReadableSize, _pktBuffer);
	_readPos = 0;
	_writePos = leftReadableSize;
}