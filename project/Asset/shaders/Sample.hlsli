#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Packages/shaders/Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

struct PSOutput {
	float4 color : SV_TARGET;
};

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint index;
};
ConstantBuffer<Parameter> gParameter : register(b0);

ConstantBuffer<Camera> gCamera : register(b1);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

