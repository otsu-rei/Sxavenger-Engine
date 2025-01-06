#include "PlayerStatePunch.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStatePunch class
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStatePunch::Init() {
	duration_ = player_->animators_[Player::AnimationState::Punching]->GetDurationTime(0);
}

void PlayerStatePunch::Term() {
}

void PlayerStatePunch::Update() {
	ActionGamepad();
	UpdateAnimation();

	if (time_ >= duration_) {
		if (nextAttackState_.has_value()) {
			player_->requestState_ = std::move(nextAttackState_.value());

		} else {
			player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
		}
	}

}

void PlayerStatePunch::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateHook>(player_);
	}
}

void PlayerStatePunch::UpdateAnimation() {
	time_ += TimePointf<TimeUnit::second>(SxavengerSystem::GetDeltaTime());

	//* player側に設定
	player_->time_ = time_;
	player_->animationState_ = Player::AnimationState::Punching;
}
