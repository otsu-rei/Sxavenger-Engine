#include "ActionGameLoop.h"

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
	player_->Init();
	player_->SetToConsole();
}

void ActionGameLoop::TermGame() {
	player_.reset();
}

void ActionGameLoop::UpdateGame() {
	player_->Update();
}

void ActionGameLoop::DrawGame() {
}
