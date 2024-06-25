#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <MyEngine.h>
#include <DirectXRCommon.h>
#include <Environment.h>

// DxrObject
#include <DxrAccelerationStructure.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	console->Init();
	Init();

	/*std::unique_ptr<Camera3D> camera = std::make_unique<Camera3D>();
	MyEngine::camera3D_ = camera.get();*/

	MyEngine::TransitionProcessSingle();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		console->Update();
		Update();

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		MyEngine::TransitionProcess();
		MyEngine::BeginDraw();

		Draw();

		MyEngine::EndFrame();

	}

	//=========================================================================================
	// 終了処理
	//=========================================================================================

	Term();
	console->Term();

}

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class private methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	defferedRendering_ = std::make_unique<DefferedRendering>();

	// particle
	particle_ = std::make_unique<Particle>();
	particle_->Init(defferedRendering_.get());

	//// TLAS 
	//tlas_ = std::make_unique<TopLevelAS>();

	//// constantBuffer
	//camera_ = std::make_unique<RayTracingCamera>();
	//light_  = std::make_unique<RayTracingLight>();

	//// Game
	//ground_ = std::make_unique<Ground>();
	//player_ = std::make_unique<Player>();
	//teapot_ = std::make_unique<Teapot>();

	//subobjectManager_ = std::make_unique<SubobjectManager>();

	//cube_ = std::make_unique<Cube>();
	//cube_->Init(subobjectManager_.get());

	//// drawer
	//fullscreen_ = std::make_unique<FullScreen>();
}

void GameScene::Term() {
}

void GameScene::Update() {

	//player_->Update();
	//camera_->Update(player_->GetWorldMatrix());
	//subobjectManager_->Update();

	//// TLASへの書き込み
	//tlas_->StartBlasSetup();

	//tlas_->SetBLAS(ground_->GetBlas(), ground_->GetWorldMatrix(), 0);
	//tlas_->SetBLAS(player_->GetBlas(), player_->GetWorldMatrix(), 0);
	//tlas_->SetBLAS(teapot_->GetBlas(), teapot_->GetWorldMatrix(), 0);
	//tlas_->SetBLAS(cube_->GetBlas(), cube_->GetWorldMatrix(), 0);

	//subobjectManager_->SetBlases(tlas_.get());

	//tlas_->EndBlasSetup();

	particle_->Update();
}

void GameScene::Draw() {

	/*auto commandList = MyEngine::GetCommandList();*/

	//=========================================================================================
	// レイトレーシング描画処理
	//=========================================================================================

	/*{
		RayTracingEngine::BeginRayTracing(tlas_.get());

		commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
		commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
		commandList->SetComputeRootConstantBufferView(2, light_->GetGPUVirtualAddress());

		RayTracingEngine::EndRayTracing();
	}

	MyEngine::TransitionProcess();
	console->OutputRayTracingResult(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());*/

	/*//-----------------------------------------------------------------------------------------
	// postEffect test
	//-----------------------------------------------------------------------------------------

	{
		blur->CreateBlurTexture(kWindowWidth, kWindowHeight, RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());
	}
	MyEngine::TransitionProcess();
	console->OutputTexture("blur", blur->GetTexture());*/

	//=========================================================================================
	// オフスクリーン描画処理
	//=========================================================================================

	{
		// todo: 複数へのtexture書き込みをさせる.
		MyEngine::BeginOffscreen(console->GetSceneTexture());
		MyEngine::camera3D_ = console->GetDebugCamera();

		/*particle_->Draw();*/

		MyEngine::EndOffscreen(console->GetSceneTexture());

		MyEngine::BeginOffscreens(kCountOfDefferedRenderingType, defferedRendering_->GetTexturePtrs());

		particle_->Draw();

		MyEngine::EndOffscreens(kCountOfDefferedRenderingType, defferedRendering_->GetTexturePtrs());

		console->OutputDefferedTextures("Deffered", DefferedRenderingType::kCountOfDefferedRenderingType, defferedRendering_->GetTextureHandles());

		MyEngine::BeginOffscreen(MyEngine::GetTexture("offscreen"));
		/*MyEngine::camera3D_ = camera.get();*/

		defferedRendering_->Draw();


		MyEngine::EndOffscreen(MyEngine::GetTexture("offscreen"));
	}

	MyEngine::TransitionProcess();

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		MyEngine::BeginScreenDraw();

		MyEngine::GetDxCommon()->CopyResource(
			MyEngine::GetDxCommon()->GetSwapChainObj()->GetResource(MyEngine::GetDxCommon()->GetSwapChainObj()->GetCurrentBackBufferIndex()), D3D12_RESOURCE_STATE_RENDER_TARGET,
			MyEngine::GetTexture("offscreen")->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
