//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DebugPrimitive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {

	PSOutput output;

	output.color = input.color;

	return output;
	
}