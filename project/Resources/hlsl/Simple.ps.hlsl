//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {

	PSOutput output = (PSOutput)0;

	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return output;
	
}