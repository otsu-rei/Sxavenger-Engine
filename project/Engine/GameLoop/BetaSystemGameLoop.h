#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test !raytracing
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/Content/TextureBuffer/UnorderedTexture.h>
#include <Engine/!Render/FPresenter.h>
#include <Engine/!Render/Scene/FSceneRenderer.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/APointLightActor.h>
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Scene/FScene.h>
#include <Engine/!Render/Scene/FPostProcessSetting.h>
#include <Engine/!Render/Scene/PostProcess/PostProcesses/FProcessLut.h>
#include <Engine/Editor/EditorComponent/AttributeComponent.h>


////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaSystemGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	//* test *//

	std::unique_ptr<FSceneTextures> textures_;

	std::unique_ptr<FScene>              scene_;
	std::unique_ptr<FPostProcessSetting> setting_;

	std::unique_ptr<FSceneRenderer> renderer_;

	//* camera *//

	std::unique_ptr<ACineCameraActor> camera_;

	//* geometries *//

	std::unique_ptr<AModelActor> model_;
	std::unique_ptr<AModelActor> model1_;

	//* light *//

	std::unique_ptr<APointLightActor> light_;

	FPresenter presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
