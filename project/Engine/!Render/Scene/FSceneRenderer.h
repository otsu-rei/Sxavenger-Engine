#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FSceneTextures.h"
#include "FScene.h"
#include "Actor/Camera/ACameraActor.h"
#include "FPostProcessSetting.h"
#include "PostProcess/FPostProcessTextures.h"

//* engine	
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>

#include "Engine/System/DirectX/DxObject/DxComputePipelineState.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneRenderer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
		bool isEmptyLightAlbedo = false;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneRenderer()  = default;
	~FSceneRenderer() = default;

	//* render *//

	void Render(const DirectXThreadContext* context);

	bool CheckRender() const;

	//* setter *//

	void SetTextures(FSceneTextures* textures) { textures_ = textures; }

	void SetScene(FScene* scene) { scene_ = scene; }

	void SetCamera(ACameraActor* camera) { camera_ = camera; }

	void SetPostProcessSetting(FPostProcessSetting* setting) { setting_ = setting; }

	//* getter *//

	Vector2ui GetSize() const { return textures_->GetSize(); }

	FScene* GetScene() const { return scene_; }

	const Config& GetConfig() const { return config_; }
	Config& GetConfig() { return config_; }

	//* debug *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetDebugTexture() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	FSceneTextures* textures_ = nullptr;

	//* scene *//

	FScene* scene_        = nullptr; //!< geometry and light actors
	ACameraActor* camera_ = nullptr; //!< camera

	//* post process *//

	FPostProcessSetting* setting_ = nullptr;

	std::unique_ptr<FPostProcessTextures> processTextures_;
	static const size_t kProcessTextureSize = 2;

	//* raytracing *//

	DxrObject::StateObjectContext stateObjectContext_;

	//* config *//

	Config config_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaqueGeometries(const DirectXThreadContext* context);

	void ProcessLighting(const DirectXThreadContext* context);

	void RenderTransparentGeometries(const DirectXThreadContext* context);

	/*void SetupRaytracing(const DirectXThreadContext* context);

	void ProcessRaytracingReflection(const DirectXThreadContext* context);*/

	void PostProcessPass(const DirectXThreadContext* context);

	//* sub method

	void RenderEmptyLight(const ALightActor::RendererContext& context);

};
