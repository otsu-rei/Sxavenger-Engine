#include "ModelInstanceBehavior.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelInstanceBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelInstanceBehavior::Init() {
}

void ModelInstanceBehavior::Term() {
}

void ModelInstanceBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {

	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	// TODO: mesh shaderへの対応
	sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->GetMesh(i).BindIABuffer();

		commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
		commandList->SetGraphicsRootShaderResourceView(1, instanceBuffer_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, model_->GetTextureHandle(i));

		model_->GetMesh(i).DrawCall(instanceBuffer_->GetIndexSize());
	}

}

void ModelInstanceBehavior::CreateInstance(uint32_t instanceSize) {
	instanceBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), instanceSize);
}
