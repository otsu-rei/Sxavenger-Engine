//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Default.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput {
	float4 color : SV_TARGET0;
};

//-----------------------------------------------------------------------------------------
// TextureBuffers
//-----------------------------------------------------------------------------------------
Texture2D<float4> gTexture : register(t5);
SamplerState gSampler : register(s0);

//=========================================================================================
// Material structure buffer
//=========================================================================================
struct Material {
	float4 color;
};
ConstantBuffer<Material> gMaterial : register(b4);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// textureSampler
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.color = gMaterial.color * textureColor;
	
	if (output.color.a == 0.0f) { //!< 透明度0の場合はpixel破棄
		discard;
	}
	
	output.color = input.color; //!< debug meshlet color
	
	return output;
}