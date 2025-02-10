#include "MaterialComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct A {
	float a;
};

struct B {
	float b;
};

struct Parameter {
	A a;
	B b;
};
//ConstantBuffer<Parameter> gParameter : register(b0);
ConstantBuffer<MaterialComponent> gMaterial : register(b1);

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)]
void main() {

	MaterialComponent material = gMaterial;
	

	gOutput[uint2(0, 0)] = float4(material.albedo.GetAlbedo(), material.transparency.GetTrnasparency());
}
