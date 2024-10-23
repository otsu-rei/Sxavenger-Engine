#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BaseRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	BaseRootSignatureDesc() { Reserve(); }

	virtual ~BaseRootSignatureDesc() { Clear(); }

	//* parameter optional *//

	void Clear();

	//* creates *//

	ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device) const;

	//=========================================================================================
	// members
	//=========================================================================================

	std::vector<D3D12_ROOT_PARAMETER>      params;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

protected:

	std::vector<D3D12_DESCRIPTOR_RANGE> ranges_;

	//* parameter *//

	static const uint32_t kMaxParamReserve_   = 32;
	static const uint32_t kMaxSamplerReserve_ = 32;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AutoResizeParam(uint32_t index);
	void AutoResizeSampler(uint32_t index);

	void Reserve();

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GraphicsRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	//* virtualAddress *//

	virtual void SetCBV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	virtual void SetVirtualSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	virtual void SetVirtualUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	//* handle *//

	virtual void SetSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	virtual void SetUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister);

	//* sampler *//

	virtual void SetSampler(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister);
	virtual void SetSampler(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister);

	virtual void SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic);

};

////////////////////////////////////////////////////////////////////////////////////////////
// CSRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CSRootSignatureDesc
	: public GraphicsRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	//* virtualAddress *//

	void SetCBV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetSRV(uint32_t index, uint32_t shaderRegister);

	void SetUAV(uint32_t index, uint32_t shaderRegister);

	//* sampler *//

	void SetSampler(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister);

	void SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister, uint32_t anisotropic);

};

_DXOBJECT_NAMESPACE_END