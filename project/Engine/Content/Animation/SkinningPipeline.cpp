#include "SkinningPipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinningComputePipeline::Init() {
	state_ = std::make_unique<ReflectionComputePipelineState>();
	state_->CreateBlob("animation/skinning.cs.hlsl");
	state_->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void SkinningComputePipeline::Term() {
}

void SkinningComputePipeline::SetPipeline(const DirectXThreadContext* context) {
	state_->ReloadAndSetPipeline(context->GetDxCommand());
}

void SkinningComputePipeline::Dispatch(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	state_->BindComputeBuffer(context->GetDxCommand(), desc);
	state_->Dispatch(context->GetDxCommand(), RoundUp(vertexSize, kNumthreads_), 1, 1);
}
