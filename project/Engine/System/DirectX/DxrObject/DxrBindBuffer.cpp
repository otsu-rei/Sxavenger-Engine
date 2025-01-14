#include "DxrBindBuffer.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void WriteBindBufferDesc::SetBuffer(uint32_t index, const std::optional<DxObject::GPUBuffer>& buffer) {
	container_[index] = buffer;
}

void WriteBindBufferDesc::SetAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	SetBuffer(index, address);
}

void WriteBindBufferDesc::SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	SetBuffer(index, handle);
}

D3D12_GPU_VIRTUAL_ADDRESS WriteBindBufferDesc::GetAddress(uint32_t index) const {
	Assert(container_[index].has_value(), "buffer is not set.");
	Assert(std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(container_[index].value()), "buffer type different.");
	return std::get<D3D12_GPU_VIRTUAL_ADDRESS>(container_[index].value());
}

D3D12_GPU_DESCRIPTOR_HANDLE WriteBindBufferDesc::GetHandle(uint32_t index) const {
	Assert(container_[index].has_value(), "buffer is not set.");
	Assert(std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(container_[index].value()), "buffer type different.");
	return std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(container_[index].value());
}

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WriteBindBufferTable::Register(uint32_t index, WriteBindBufferType type) {
	table_[index] = type;
}

void WriteBindBufferTable::RegisterAddress(uint32_t index) {
	Register(index, WriteBindBufferType::VirtualAddress);
}

void WriteBindBufferTable::RegisterHandle(uint32_t index) {
	Register(index, WriteBindBufferType::DescriptorHandle);
}

WriteBindBufferDesc::Container WriteBindBufferTable::GetWriteBuffers(const WriteBindBufferDesc& desc) const {
	WriteBindBufferDesc::Container buffers(table_.size());

	for (uint32_t i = 0; i < table_.size(); ++i) {
		switch (table_[i]) {
			case WriteBindBufferType::VirtualAddress:
				buffers[i] = desc.GetAddress(i);
				break;

			case WriteBindBufferType::DescriptorHandle:
				buffers[i] = desc.GetHandle(i);
				break;
		}
	}

	return buffers;
}
