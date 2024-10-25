#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Camera structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Camera {
	float4   position;
	float4x4 viewMatrix;
	float4x4 worldMatrix;
	float4x4 projMatrix;
	float4x4 projInverseMatrix;
};