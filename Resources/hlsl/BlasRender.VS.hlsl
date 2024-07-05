//*****************************************************************************************
// MS, PS パイプラインに変更
//*****************************************************************************************

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "BlasRender.hlsli"
#include "Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// VSInput sturucture
////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

//=========================================================================================
// Buffers
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, viewProj);
	output.normal = normalize(input.normal);
	
	return output;
	
}