//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DebugPrimitive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {

	PSOutput output;

	if (input.position.z <= 0.0f || input.color.a == 0.0f) {
		discard;
	}

	output.color = input.color;

	return output;
	
}