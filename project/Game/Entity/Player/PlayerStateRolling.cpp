#include "PlayerStateRolling.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateRolling::Init() {
	duration_ = player_->animators_[Player::AnimationState::Rolling]->GetDurationTime(0);
	player_->hitCollider_->SetIsActive(false);
}

void PlayerStateRolling::Term() {
	time_ = TimePointf<TimeUnit::second>(0);
	player_->hitCollider_->SetIsActive(true);
}

void PlayerStateRolling::Update() {
	time_ += TimePointf<TimeUnit::second>(SxavengerSystem::GetDeltaTime());

	//* player側に設定
	player_->time_ = time_;
	player_->animationState_ = Player::AnimationState::Rolling;

	if (time_ >= duration_) {
		player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
	}

	static const Vector3f base = { 0.0f, 0.0f, 1.0f };
	Vector3f direction = RotateVector(base, player_->target_);

	player_->GetTransform().translate += direction * speed_ * SxavengerSystem::GetDeltaTime().time;
}
