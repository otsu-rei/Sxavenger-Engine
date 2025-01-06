#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Console/Console.h>

//* lib
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////
void Player::Init() {

	//* external *//

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	camera_ = sConsole->GetGameCamera();

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

	animators_[AnimationState::Idle]     = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/idle.gltf").lock();
	animators_[AnimationState::Walking]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/walking.gltf").lock();
	animators_[AnimationState::Running]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/running.gltf").lock();
	animators_[AnimationState::Rolling]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/rolling.gltf").lock();
	animators_[AnimationState::Punching] = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/punching.gltf").lock();
	animators_[AnimationState::Hooking]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/hooking.gltf").lock();
	animators_[AnimationState::Elbow]    = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/elbow.gltf").lock();
	animators_[AnimationState::Straight] = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/straight.gltf").lock();

	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { SxavengerAsset::PushTask(animator); });
	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { animator->WaitComplete(); });

	animationState_ = AnimationState::Idle;

	//* collider *//

	hitCollider_ = std::make_unique<Collider>();
	hitCollider_->SetToCollection();

	hitCollider_->SetColliderBoundingCapsule({
		{0.0f, 1.0f, 0.0f}, //!< direction
		0.3f, 1.0f
	});
	hitCollider_->SetParent(this);
	hitCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.0f };
}

void Player::Term() {
}

void Player::Update() {
	UpdateState();

	ModelBehavior::UpdateMatrix();
	hitCollider_->UpdateMatrix();
	UpdateCamera();

	//* update skeleton *//

	UpdateAnimation();
}

void Player::SetAttributeImGui() {
	if (ImGui::TreeNode("collider")) {
		hitCollider_->SetImGuiCommand();
		ImGui::TreePop();
	}
}

void Player::UpdateState() {
	//!< apply request state
	if (requestState_.has_value()) {
		state_->Term(); //!< 前のstateを終了

		state_ = std::move(requestState_.value()); //!< request stateを適用
		state_->Init();

		requestState_ = std::nullopt;
	}

	//!< update state
	state_->Update();
}

void Player::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();
	skeleton_->UpdateAnimation(animators_[animationState_]->GetAnimation(0), time_);
}

void Player::UpdateCamera() {
	pivot_ = GetPosition();
	pivotRotation_.x = std::fmod(pivotRotation_.x, pi_v * 2.0f);
	pivotRotation_.y = std::clamp(pivotRotation_.y, 0.0f, pi_v / 16.0f);

	Quaternion rotate
		= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pivotRotation_.x)
		* MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, pivotRotation_.y);

	static const Vector3f direction = { 0.0f, 0.0f, -1.0f };
	Vector3f rotatedDirection = RotateVector(direction, rotate);

	camera_->GetTransform().translate = pivot_ + rotatedDirection * distance_ + offset_;
	camera_->GetTransform().rotate    = rotate;
	camera_->UpdateMatrix();
}
