#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* DirectX12
#include <dxcapi.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderCompiler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderCompiler()  = default;
	~ShaderCompiler() { Term(); }

	void Init();

	void Term();

	//* compiler opiton *//

	ComPtr<IDxcBlob> Compile(
		const std::wstring& filepath,
		const std::wstring& profile,
		const std::wstring& entryPoint = L""
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compiler *//

	ComPtr<IDxcUtils>          utils_;
	ComPtr<IDxcCompiler3>      compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

};

_DXOBJECT_NAMESPACE_END