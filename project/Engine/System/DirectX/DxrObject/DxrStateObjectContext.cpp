#include "DxrStateObjectContext.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure
////////////////////////////////////////////////////////////////////////////////////////////

void StateObjectDesc::SetBlob(const DxrObject::RaytracingBlob* blob) {
	blobs.emplace(blob);
}

void StateObjectDesc::SetExportParameter() {
	//!< 初期化
	exports_    = {};
	maxStrides_ = {};

	for (const auto& blob : blobs) {
		for (const auto& expt : blob->GetExports()) {

			ExportType type = expt->GetType();
			Assert(!(type == ExportType::Raygeneration && exports_[static_cast<size_t>(type)].size() == 1), "raytracing enviorment can set one raygeneration.");
			//!< raygenerationは1つしか含まれない

			exports_[static_cast<size_t>(type)].emplace(expt);
			maxStrides_[static_cast<size_t>(type)] = std::max(maxStrides_[static_cast<size_t>(type)], expt->GetBufferStride());
		}
	}
}

void StateObjectDesc::SetPayloadStride(size_t stride) {
	payloadStride = stride;
}

void StateObjectDesc::SetAttributeStride(size_t stride) {
	attributeStride = stride;
}

void StateObjectDesc::SetMaxRecursionDepth(uint8_t depth) {
	Assert(depth > 0 && depth < D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH, "recursion depth is out of range.");
	maxRecursionDepth = depth;
}

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StateObjectContext::CreateRootSignature(DxObject::Device* device, const GlobalRootSignatureDesc& desc) {
	rootSignature_ = desc.CreateGlobalRootSignature(device->GetDevice());
}

void StateObjectContext::CreateStateObject(DxObject::Device* device, const StateObjectDesc& desc) {

	// descの保存
	desc_ = desc;

	// state objectの設定
	CD3DX12_STATE_OBJECT_DESC stateObjectDesc = {};
	stateObjectDesc.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// subobjectの設定
	BindDXGILibrarySubobject(stateObjectDesc);
	BindGlobalRootSignatureSubobject(stateObjectDesc);
	BindExportLocalRootSignatureSubobject(stateObjectDesc);
	BindConfigsSubobject(stateObjectDesc);

	// state objectの生成
	auto hr = device->GetDevice()->CreateStateObject(
		stateObjectDesc, IID_PPV_ARGS(&stateObject_)
	);
	Assert(SUCCEEDED(hr));
	// note: DebugLayer = true でPIXを起動するとここでエラーが発生する.

	// propertiesの取得
	stateObject_.As(&properties_);

}

void StateObjectContext::UpdateShaderTable(DxObject::Device* device) {

	// shader単体のsizeの設定
	UINT raygenerationRecordSize = kShaderRecordSize;
	raygenerationRecordSize     += static_cast<UINT>(desc_.GetMaxStride(ExportType::Raygeneration));
	raygenerationRecordSize      = Alignment(raygenerationRecordSize, kShaderRecordAlignment);

	// miss
	UINT missRecordSize = kShaderRecordSize;
	missRecordSize     += static_cast<UINT>(desc_.GetMaxStride(ExportType::Miss));
	missRecordSize      = Alignment(missRecordSize, kShaderRecordAlignment);

	// hitgroup
	UINT hitgroupRecordSize = kShaderRecordSize;
	hitgroupRecordSize     += static_cast<UINT>(desc_.GetMaxStride(ExportType::Hitgroup));
	hitgroupRecordSize      = Alignment(hitgroupRecordSize, kShaderRecordAlignment);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める.
	UINT raygenerationSize = static_cast<UINT>(desc_.GetCount(ExportType::Raygeneration))* raygenerationRecordSize;
	UINT missSize          = static_cast<UINT>(desc_.GetCount(ExportType::Miss)) * missRecordSize;
	UINT hitgroupSize      = static_cast<UINT>(desc_.GetCount(ExportType::Hitgroup)) * hitgroupRecordSize;

	// 各テーブル開始位置にアライメント調整
	UINT raygenerationRegion = Alignment(raygenerationSize, kShaderTableAlignment);
	UINT missRegion          = Alignment(missSize,          kShaderTableAlignment);
	UINT hitgroupRegion      = Alignment(hitgroupSize,      kShaderTableAlignment);

	// 合計したtableのサイズ
	UINT tableSize = raygenerationRegion + missRegion + hitgroupRegion;

	if (tableSize > shaderTableStride_) { //!< tableのサイズが大きい場合
		shaderTableStride_ = tableSize;
		shaderTable_.Reset();

		// tableのresourceの生成
		shaderTable_ = DxObject::CreateBufferResource(
			device->GetDevice(),
			D3D12_HEAP_TYPE_UPLOAD,
			shaderTableStride_,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);
	}

	// tableのmap
	void* mapped = nullptr;
	shaderTable_->Map(0, nullptr, &mapped);
	uint8_t* addressStart = static_cast<uint8_t*>(mapped);

	// raygeneration
	{
		uint8_t* address = addressStart;

		for (const auto& expt : desc_.GetExports(ExportType::Raygeneration)) {
			address = WriteExport(address, raygenerationRecordSize, expt);
		}
	}

	// miss
	{
		uint8_t* address = addressStart + raygenerationRegion;

		for (const auto& expt : desc_.GetExports(ExportType::Miss)) {
			address = WriteExport(address, missRecordSize, expt);
		}
	}

	// hitgroup
	{
		uint8_t* address = addressStart + raygenerationRegion + missRegion;

		for (const auto& expt : desc_.GetExports(ExportType::Hitgroup)) {
			address = WriteExport(address, hitgroupRecordSize, expt); //!< todo: desc
		}
	}

	shaderTable_->Unmap(0, nullptr);

	// descの設定
	dispatchDesc_ = {};

	auto address = shaderTable_->GetGPUVirtualAddress();

	// raygenerationの設定
	dispatchDesc_.RayGenerationShaderRecord.StartAddress = address;
	dispatchDesc_.RayGenerationShaderRecord.SizeInBytes  = raygenerationSize;
	address += raygenerationRegion;

	// missの設定
	dispatchDesc_.MissShaderTable.StartAddress  = address;
	dispatchDesc_.MissShaderTable.SizeInBytes   = missSize;
	dispatchDesc_.MissShaderTable.StrideInBytes = missRecordSize;
	address += missRegion;

	// hitgroupの設定
	if (desc_.GetCount(ExportType::Hitgroup) != 0) {
		dispatchDesc_.HitGroupTable.StartAddress  = address;
		dispatchDesc_.HitGroupTable.SizeInBytes   = hitgroupSize;
		dispatchDesc_.HitGroupTable.StrideInBytes = hitgroupRecordSize;
		address += hitgroupRegion;
	}
	
}

void StateObjectContext::SetStateObject(DxObject::CommandContext* context) {
	context->GetCommandList()->SetComputeRootSignature(rootSignature_.Get());
	context->GetCommandList()->SetPipelineState1(stateObject_.Get());
}

void StateObjectContext::DispatchRays(DxObject::CommandContext* context, const Vector2ui& size) {

	dispatchDesc_.Width  = size.x;
	dispatchDesc_.Height = size.y;
	dispatchDesc_.Depth  = 1;

	context->GetCommandList()->DispatchRays(&dispatchDesc_);
}

void StateObjectContext::BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc) {
	for (const auto& blob : desc_.blobs) {
		// subobjectの作成
		auto subobject = desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		// blobの設定
		D3D12_SHADER_BYTECODE bytecode = blob->GetBytecode();
		subobject->SetDXILLibrary(&bytecode);

		// entry pointの設定
		std::vector<LPCWSTR> entries = {};

		// exportからentryを取得
		for (const auto& expt : blob->GetExports()) {
			switch (expt->GetType()) {
				case ExportType::Raygeneration:
					// raygenerationのentryを取得
					entries.emplace_back(expt->GetName().c_str());
					break;

				case ExportType::Miss:
					// missのentryを取得
					entries.emplace_back(expt->GetName().c_str());
					break;

				case ExportType::Hitgroup:
					// hitgroupのentryを取得
					const auto& hitgroup = expt->GetHitgroup();

					entries.emplace_back(hitgroup.closesthit.c_str());

					if (!hitgroup.anyhit.empty()) {
						entries.emplace_back(hitgroup.anyhit.c_str());
					}

					if (!hitgroup.intersection.empty()) {
						entries.emplace_back(hitgroup.intersection.c_str());
					}

					break;
			}
		}

		subobject->DefineExports(entries.data(), static_cast<UINT>(entries.size()));
	}
}

void StateObjectContext::BindGlobalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {
	auto subobject = desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	subobject->SetRootSignature(rootSignature_.Get());
}

void StateObjectContext::BindExportLocalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {
	for (const auto& blob : desc_.blobs) {
		for (const auto& expt : blob->GetExports()) {

			// hitgroupの場合
			if (expt->GetType() == ExportType::Hitgroup) {

				// hitgroupのexportを設定
				auto hitgroupSubobject = desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				hitgroupSubobject->SetHitGroupExport(expt->GetName().c_str());
				hitgroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
				// todo: AABB Geometryの対応

				// entry pointの設定
				const auto& hitgroup = expt->GetHitgroup();

				Assert(!hitgroup.closesthit.empty(), "closest hit is empty.");
				hitgroupSubobject->SetClosestHitShaderImport(hitgroup.closesthit.c_str());

				if (!hitgroup.anyhit.empty()) {
					hitgroupSubobject->SetAnyHitShaderImport(hitgroup.anyhit.c_str());
				}

				if (!hitgroup.intersection.empty()) {
					hitgroupSubobject->SetIntersectionShaderImport(hitgroup.intersection.c_str());
				}
			}

			ID3D12RootSignature* localRootSignature = expt->GetRootSignature();

			if (localRootSignature == nullptr) {
				continue;
			}

			// localRootSignatureの設定
			auto localRootSignatureSubobject = desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignatureSubobject->SetRootSignature(localRootSignature);

			// export関連付け
			auto exportSubobject = desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			exportSubobject->AddExport(expt->GetName().c_str());
			exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);
		}
	}
}

void StateObjectContext::BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto pipelineConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfigSubobject->Config(desc_.maxRecursionDepth);

	auto shaderConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfigSubobject->Config(static_cast<UINT>(desc_.payloadStride), static_cast<UINT>(desc_.attributeStride));

}

UINT StateObjectContext::WriteIdentifier(uint8_t* dst, const void* id) {
	std::memcpy(dst, id, kShaderRecordSize);
	return kShaderRecordSize;
}

UINT StateObjectContext::WriteGPUVirtualAddress(uint8_t* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	std::memcpy(dst, &address, kBufferStride);
	return kBufferStride;
}

UINT StateObjectContext::WriteGPUHandle(uint8_t* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	std::memcpy(dst, &handle, kBufferStride);
	return kBufferStride;
}

uint8_t* StateObjectContext::WriteExport(uint8_t* dst, UINT size, const ExportGroup* expt, const WriteBindBufferDesc* desc) {

	auto end = dst + size;

	// exportのid書き込み
	auto id = properties_->GetShaderIdentifier(expt->GetName().c_str());
	AssertW(id != nullptr, L"export identifier not found. export name: " + expt->GetName());

	dst += WriteIdentifier(dst, id);

	// descの書き込み
	if (desc == nullptr) {
		return end; //!< bufferがない場合
	}

	for (const auto& buffer : expt->GetTable().GetWriteBuffers(*desc)) {
		Assert(buffer.has_value(), "buffer not has value.", "// maybe system error.");
		const auto& value = buffer.value();
		
		if (std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(value)) {
			dst += WriteGPUVirtualAddress(dst, std::get<D3D12_GPU_VIRTUAL_ADDRESS>(value));

		} else if (std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(value)) {
			dst += WriteGPUHandle(dst, std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(value));
		}
	}

	return end;
}
