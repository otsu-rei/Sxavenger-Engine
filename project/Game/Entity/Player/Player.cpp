#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////
void Player::Init() {

	//* external *//

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	//* state *//

	state_ = std::make_unique<PlayerStateRoot>(this);
	state_->Init();

	ModelBehavior::SetName("player");
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	ModelBehavior::model_ = SxavengerAsset::TryImport<Model>("asset/model/sample/idle.gltf");
	SxavengerAsset::PushTask(ModelBehavior::model_.value().Lock());
	ModelBehavior::model_.value().Lock()->WaitComplete();

	skeleton_ = std::make_unique<SkeletonMesh>();
	skeleton_->Create(model_.value().Lock());
	AnimationBehavior::skeletonMesh_ = skeleton_.get();

	//* animation *//

	animators_[AnimationState::Idle]    = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/idle.gltf").lock();
	animators_[AnimationState::Walking] = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/walking.gltf").lock();

	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { SxavengerAsset::PushTask(animator); });
	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { animator->WaitComplete(); });

	animationState_ = AnimationState::Idle;
}

void Player::Term() {
}

void Player::Update() {
	UpdateState();

	ModelBehavior::UpdateMatrix();

	//* update skeleton *//

	UpdateAnimation();
}

void Player::UpdateState() {
	//!< apply request state
	if (requestState_.has_value()) {
		state_->Term(); //!< 前のstateを終了

		state_ = std::move(requestState_.value()); //!< request stateを適用
		state_->Init();
	}

	//!< update state
	state_->Update();
}

void Player::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();
	skeleton_->UpdateAnimation(animators_[animationState_]->GetAnimation(0), time_);
}
