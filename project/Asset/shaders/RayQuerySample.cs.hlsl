//=========================================================================================
// buffers
//=========================================================================================

RaytracingAccelerationStructure gScene : register(t0);


////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint2 kImageSize = uint2(1280, 720);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	uint2 size  = kImageSize;

	if (any(index >= size)) {
		return;
	}
	
	RayQuery<RAY_FLAG_CULL_BACK_FACING_TRIANGLES> q;
	
	
	
}
