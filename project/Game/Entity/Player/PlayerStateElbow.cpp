#include "PlayerStateElbow.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateElbow class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateElbow::Init() {
	duration_ = player_->animators_[Player::AnimationState::Elbow]->GetDurationTime(0);
}

void PlayerStateElbow::Term() {
}

void PlayerStateElbow::Update() {
	UpdateAnimation();

	if (time_ >= duration_) {
		player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
	}
}

void PlayerStateElbow::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();

	//* player側に設定
	player_->time_ = time_;
	player_->animationState_ = Player::AnimationState::Elbow;
}
