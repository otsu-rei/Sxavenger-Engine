#include "DxBufferResource.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BufferResourceBase class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBufferResource::Init(uint32_t indexSize, size_t structureSize) {
	Assert((structureSize > 0)); //!< 引数の確認

	indexSize_     = indexSize;
	structureSize_ = static_cast<UINT>(structureSize);
}

void BaseBufferResource::Term() {
	indexSize_     = NULL;
	structureSize_ = NULL;
}

bool BaseBufferResource::CheckElementCount(uint32_t elementCount) const {
	if (elementCount >= indexSize_) {
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// IndexBufferResource methods
////////////////////////////////////////////////////////////////////////////////////////////

void IndexBufferResource::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);

	// resourceをマッピング
	uint32_t* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, indexSize_ };
}

void IndexBufferResource::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// OffsetBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void OffsetBufferResource::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = CreateBufferResource(
		device,
		structureSize_
	);

	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedDatas_));
}

void OffsetBufferResource::Term() {
}
