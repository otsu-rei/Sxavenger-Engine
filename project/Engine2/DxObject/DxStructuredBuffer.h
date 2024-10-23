#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>

// DxObject
#include <DxDevices.h>
#include <DxBufferResource.h>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// StructuredBuffer class
	////////////////////////////////////////////////////////////////////////////////////////////
	class StructuredBuffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		StructuredBuffer(BaseBufferResource* bufferResource) {
			Init(bufferResource);
		}

		~StructuredBuffer() { Term(); }

		void Init(BaseBufferResource* bufferResource);

		void Term();

		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return descriptor_.GetGPUHandle(); }

		const Descriptor& GetDescriptor() const { return descriptor_; }

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		Descriptor descriptor_; //!< SRV descriptor

	};

}
