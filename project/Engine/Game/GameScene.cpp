#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	Init();

	Sxavenger::ResetBackAllocator();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (Sxavenger::ProcessMessage() == 0) {

		Sxavenger::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		sSystemConsole->UpdateConsole();

		if (sSystemConsole->IsUpdateRequired()) {
			Update();
		}

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		Sxavenger::TranstionAllocator();

		sSystemConsole->Draw();
		Draw();

		Sxavenger::EndImGuiAndDrawCall();
		Sxavenger::EndFrame();

		SxavengerGame::Reset();
	}

	//=========================================================================================
	// 終了処理
	//=========================================================================================

	Term();

}

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class private methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetToConsole();

	atmosphericScattering_ = std::make_unique<AtmosphericScattering>();
	atmosphericScattering_->Init();
	atmosphericScattering_->SetToConsole();

	enemyCollection_ = std::make_unique<EnemyCollection>();
	enemyCollection_->Init();
	enemyCollection_->SetToConsole();
	enemyCollection_->SetPlayer(player_.get());

	field_ = std::make_unique<Field>();
	field_->Init();
	field_->SetToConsole();
}

void GameScene::Term() {
}

void GameScene::Update() {
	player_->Update();
	enemyCollection_->Update();

	SxavengerGame::UpdateColliders();
}

void GameScene::Draw() {

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		Sxavenger::BeginScreenDraw();
		
		// screenにframeを描画
		//!< screenに描画する画面は [MainMenu > Config] から設定
		sSystemConsole->PresentToScreen();
		
	}
}
