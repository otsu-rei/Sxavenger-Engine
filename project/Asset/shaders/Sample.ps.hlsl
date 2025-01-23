//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Sample.hlsli"

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {

	PSOutput output = (PSOutput)0;

	Texture2D<float4> texture = ResourceDescriptorHeap[gParameter.index];

	output.color = texture.Sample(gSampler, input.texcoord);

	return output;
	
}
