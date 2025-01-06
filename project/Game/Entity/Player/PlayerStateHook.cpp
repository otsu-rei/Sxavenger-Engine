#include "PlayerStateHook.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateHooking class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateHook::Init() {
	duration_ = player_->animators_[Player::AnimationState::Hooking]->GetDurationTime(0);
}

void PlayerStateHook::Term() {
}

void PlayerStateHook::Update() {
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

void PlayerStateHook::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateElbow>(player_);
	}
}

void PlayerStateHook::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();

	//* player側に設定
	player_->time_ = time_;
	player_->animationState_ = Player::AnimationState::Hooking;
}
