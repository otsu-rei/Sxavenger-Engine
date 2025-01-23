//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>

//* lib


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

	std::shared_ptr<AssetTexture>                        texture_;
	std::unique_ptr<DxObject::DimensionBuffer<uint32_t>> index_;

	std::unique_ptr<ACineCameraActor> camera_;

	TriangleInputAssembler<std::pair<Vector4f, Vector2f>> triangle_;

	std::unique_ptr<DxObject::GraphicsPipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
