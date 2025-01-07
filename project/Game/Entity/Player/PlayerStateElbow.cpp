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
	player_->SetAnimationState(Player::AnimationState::Elbow);

	attackCollider_ = std::make_unique<Collider>();
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.2f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();

}

void PlayerStateElbow::Term() {
}

void PlayerStateElbow::Update() {
	UpdateAnimation();
	ActionGamepad();

	if (time_ >= duration_) {
		if (nextAttackState_.has_value()) {
			player_->requestState_ = std::move(nextAttackState_.value());
			
		} else {
			player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
		}
	}
}

void PlayerStateElbow::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateStraight>(player_);
	}
}

void PlayerStateElbow::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();

	//* player側に設定
	player_->time_ = time_;
}
