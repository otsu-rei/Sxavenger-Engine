#include "ActionGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ActionGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void ActionGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermGame(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateGame(); });
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawGame(); });
}

void ActionGameLoop::Term() {
}

void ActionGameLoop::InitGame() {
	player_ = std::make_unique<Player>();

	QuaternionTransform transform = {};
	transform.translate = { 0.0f, 0.0f, -8.0f };
	transform.rotate = Quaternion::Identity();
	player_->Init(transform);
	player_->SetToConsole();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();
	enemy_->SetToConsole();

	ground_ = std::make_unique<Ground>();
	ground_->Init();
	ground_->SetToConsole();
}

void ActionGameLoop::TermGame() {
	player_.reset();
	enemy_.reset();
	ground_.reset();
}

void ActionGameLoop::UpdateGame() {
	player_->Update();
	enemy_->Update();

	SxavengerModule::CheckCollision();
}

void ActionGameLoop::DrawGame() {
}
