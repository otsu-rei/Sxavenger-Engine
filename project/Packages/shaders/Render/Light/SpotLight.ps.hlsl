//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct SpotLight {
	float4 color_intensity; //!< rgb : color, a : intensity
	float distance;
	float falloff; //!< theta_p
	float angle;   //!< theta_u
};
ConstantBuffer<SpotLight> gSpotLight : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass���̎擾
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Light�̏����擾
	float3 p_light = gTransform[input.instanceId].GetPosition(); //!< light�̒��S���W
	
	//* �v�Z
	//!< func_diffuse(n, l)
	float3 l      = normalize(p_light - surface.position); //!< light�̕����x�N�g��
	float diffuse = CalculateDiffuseHalfLambert(surface.normal, l);
	
	//!< func_dist(r) = func_win(r);
	float r    = length(p_light - surface.position); //!< light��surface�̋���
	float dist = pow(max(1.0f - pow(r / gSpotLight.distance, 4.0f), 0.0f), 2.0f); //!< dist = func_win(r);
	
	//!< func_dir(l)
	float theta_s = dot(-l, gTransform[input.instanceId].GetDirection());
	float t       = saturate((theta_s - gSpotLight.angle) / (gSpotLight.falloff - gSpotLight.angle));
	//float dir     = t * t;
	float dir     = t * t * (3.0f - 2.0f * t); //!< smoothstep

	float3 c_light = gSpotLight.color_intensity.rgb * gSpotLight.color_intensity.a * dist * dir;
	
	RayQuery<0> q;
	
	RayDesc ray;
	ray.Origin    = surface.position;
	ray.Direction = l;
	ray.TMin      = 0.001f;
	ray.TMax      = r;
	
	q.TraceRayInline(
		gScene,
		RAY_FLAG_NONE,
		0xFF,
		ray
	);
	
	if (q.Proceed()) {
		c_light /= 2.0f;
	}
	
	//* �o��
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
	
}

