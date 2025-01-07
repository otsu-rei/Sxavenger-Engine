#include "PlayerStateKick.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateKick::Init() {
	duration_ = player_->animators_[Player::AnimationState::Kick]->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Kick);
}

void PlayerStateKick::Term() {
}

void PlayerStateKick::Update() {
	UpdateAnimation();

	if (time_ >= duration_) {
		player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
	}
}

void PlayerStateKick::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();

	//* player側に設定
	player_->time_ = time_;
}
