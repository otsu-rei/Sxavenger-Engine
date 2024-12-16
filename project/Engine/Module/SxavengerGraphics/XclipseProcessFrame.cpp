#include "XclipseProcessFrame.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// XclipseProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void XclipseProcessFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_UAV | VIEWFLAG_SRV, size, {}, kOffscreenFormat);
	}
}

void XclipseProcessFrame::Term() {
}

void XclipseProcessFrame::TransitionBeginProcess(const DirectXThreadContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};

	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void XclipseProcessFrame::TransitionEndProcess(const DirectXThreadContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};
	
	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void XclipseProcessFrame::NextResultBufferIndex() {
	resultBufferIndex_++;
	resultBufferIndex_ %= kProcessBufferNum_;
}

void XclipseProcessFrame::ResetResultBufferIndex() {
	resultBufferIndex_ = 0;
}

MultiViewTexture* XclipseProcessFrame::GetPrevBuffer(uint32_t prev) const {
	Assert(prev < kProcessBufferNum_, "visual: prev over process buffer.");

	uint32_t index = (resultBufferIndex_ + kProcessBufferNum_ - prev) % kProcessBufferNum_;
	return buffers_.at(index).get();
}
