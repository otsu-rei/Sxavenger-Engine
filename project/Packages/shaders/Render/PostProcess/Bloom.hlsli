#pragma once

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint2 size;     //!< blur size
	float threshold; //!< 臒l note: float3�ɂ��邩��
};
ConstantBuffer<Parameter> gParameter : register(b0);
//!< b11�ɕύX���邩��
