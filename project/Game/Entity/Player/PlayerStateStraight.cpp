#include "PlayerStateStraight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateStraight::Init() {
	duration_ = player_->animators_[Player::AnimationState::Straight]->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Straight);

	attackCollider_ = std::make_unique<Collider>();
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.0f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();

}

void PlayerStateStraight::Term() {
}

void PlayerStateStraight::Update() {
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

void PlayerStateStraight::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateKick>(player_);
	}
}

void PlayerStateStraight::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime().time * 1.4f;
	//* player側に設定
	player_->time_ = time_;
}
