#include "packet_buffer_manager.h"

PacketBufferManager* PacketBufferManager::instance_ = nullptr;
std::mutex PacketBufferManager::mutex_;

// Singleton (parellel reactor에서 하나의 버퍼에 담기 위해)
PacketBufferManager* PacketBufferManager::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new PacketBufferManager();
	}
	return instance_;
}

PacketBufferManager::~PacketBufferManager() {
	delete[] packet_buffer_;
	delete[] packet_buffer_temp_;
};

void PacketBufferManager::Init(uint32_t buffer_size, uint32_t header_size, uint32_t max_packet_size)
{
	packet_buffer_ = new char[buffer_size];
	packet_buffer_temp_ = new char[buffer_size];
	buffer_size_ = buffer_size;
	header_size_ = header_size;
	max_packet_size_ = max_packet_size;
}

bool PacketBufferManager::Write(char* data, uint32_t size)
{
	// 하나의 스레드만 버퍼에 접근하도록 락
	std::lock_guard<std::mutex> lock(mutex_);
	
	auto remain_buffer_size = buffer_size_ - write_pos_;
	if (remain_buffer_size < size) {
		std::cout << "Buffer is full" << std::endl;
		return false;
	}

	std::copy(data, data + size, packet_buffer_ + write_pos_);
	write_pos_ += size;

	if (HasEnoughBufferSpace() == false) {
		BufferRelocate();
	}
	return true;
}

char* PacketBufferManager::Read() {
	// 하나의 스레드만 버퍼에 접근하도록 락
	std::lock_guard<std::mutex> lock(mutex_);

	auto readable_size = write_pos_ - read_pos_;
	if (readable_size < header_size_) {
		return nullptr;
	}

	auto packet_size = ((packet_buffer_ + read_pos_)[1] << 8) | ((packet_buffer_ + read_pos_)[0]);
	std::cout<< "pktSize : " << packet_size << std::endl;

	if (readable_size < packet_size) {
		return nullptr;
	}

	auto packet_data = new char[packet_size];
	std::copy(packet_buffer_ + read_pos_, packet_buffer_ + read_pos_ + packet_size, packet_data);
	read_pos_ += packet_size;
	return packet_data;
}

bool PacketBufferManager::HasEnoughBufferSpace()
{
	return (buffer_size_ - read_pos_) >= max_packet_size_;
}

void PacketBufferManager::BufferRelocate()
{
	std::copy(packet_buffer_ + read_pos_, packet_buffer_ + write_pos_, packet_buffer_temp_);

	auto readable_size = write_pos_ - read_pos_;
	std::copy(packet_buffer_temp_, packet_buffer_temp_ + readable_size, packet_buffer_);

	read_pos_ = 0;
	write_pos_ = readable_size;
}