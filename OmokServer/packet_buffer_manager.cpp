#include "packet_buffer_manager.h"

std::mutex PacketBufferManager::mutex_;
char* PacketBufferManager::packet_buffer_;
char* PacketBufferManager::packet_buffer_temp_;
uint32_t PacketBufferManager::buffer_size_;
uint32_t PacketBufferManager::read_pos_;
uint32_t PacketBufferManager::write_pos_;
uint32_t PacketBufferManager::header_size_;
uint32_t PacketBufferManager::max_packet_size_;

void PacketBufferManager::ClearBuffer() {
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
		std::print("Buffer is full\n");
		return false;
	}

	std::copy(data, data + size, packet_buffer_ + write_pos_);
	write_pos_ += size;

	if (HasEnoughBufferSpace()  == false) {
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

	char size[2] = { (packet_buffer_ + read_pos_)[0], (packet_buffer_ + read_pos_)[1] };
	auto packet_size = *reinterpret_cast<uint16_t*>(size);

	std::print("pktSize : {}\n", packet_size);

	if (readable_size < packet_size) {
		return nullptr;
	}

	auto packet_data = packet_buffer_ + read_pos_;
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