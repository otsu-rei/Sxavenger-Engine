#include "SceneGame.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneGame::Init() {

	component_.Init();

	camera_ = std::make_unique<GameCamera>();
	camera_->Init();
	component_.renderer_->SetCamera(camera_.get());

	player_ = std::make_unique<Player>();

	QuaternionTransform transform = {};
	transform.translate = { 0.0f, 0.0f, -8.0f };
	transform.rotate = Quaternion::Identity();
	player_->Init(transform);
	player_->SetToOutliner();

	component_.scene_->AddGeometry(player_.get());

	/*enemy_ = std::make_unique<Enemy>();
	enemy_->Init();
	enemy_->SetToConsole();

	ground_ = std::make_unique<Ground>();
	ground_->Init();
	ground_->SetToConsole();*/

	light_ = std::make_unique<ADirectionalLightActor>();
	light_->Init();

	component_.scene_->AddLight(light_.get());
}

void SceneGame::Update() {

	camera_->Update();
	player_->Update();

	//player_->Update();
	//enemy_->Update();

	//SxavengerModule::CheckCollision();

	//if (enemy_->IsDead()) {
	//	controller_->RequestNextScene("Title");
	//}
}

void SceneGame::Draw() {
}

void SceneGame::Term() {
}
