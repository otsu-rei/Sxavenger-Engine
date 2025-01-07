#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/SxavengerModule.h>

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
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kLateAdaptive);

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
	animators_[AnimationState::Kick]	 = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/kick.gltf").lock();

	//std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { SxavengerAsset::PushTask(animator); });
	//std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { animator->WaitComplete(); });

	for (size_t i = 0; i < animators_.size(); ++i) {
		SxavengerAsset::PushTask(animators_[i]);
		animators_[i]->WaitComplete();
	}

	//animationState_ = AnimationState::Idle;

	//* collider *//

	hitCollider_ = std::make_unique<Collider>();
	hitCollider_->SetToCollection();

	hitCollider_->SetColliderBoundingCapsule({
		{0.0f, 1.0f, 0.0f}, //!< direction
		0.3f, 1.0f
	});
	hitCollider_->SetParent(this);
	hitCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.0f };
	hitCollider_->SetTypeId(ColliderType::kPlayer);

	dof_ = std::make_unique<VisualDoF>();
	dof_->Init();
	dof_->SetToConsole();
	dof_->GetParameter().f           = 30.0f;
	dof_->GetParameter().focusLength = 5.0f;


	texture_ = SxavengerAsset::TryImport<Texture>("asset/textures/ui.png").Lock();
	SxavengerAsset::PushTask(texture_);
	texture_->WaitComplete();

}

void Player::Init(const QuaternionTransform& transform) {
	Init();
	ModelBehavior::GetTransform() = transform;
	ModelBehavior::UpdateMatrix();
	hitCollider_->UpdateMatrix();
	UpdateCamera();
}

void Player::Term() {
}

void Player::Update() {
	UpdateState();


	ModelBehavior::GetTransform().translate.x = std::clamp(ModelBehavior::GetTransform().translate.x, -11.0f, 11.0f);
	ModelBehavior::GetTransform().translate.z = std::clamp(ModelBehavior::GetTransform().translate.z, -11.0f, 11.0f);

	ModelBehavior::UpdateMatrix();
	hitCollider_->UpdateMatrix();
	UpdateCamera();

	//* update skeleton *//

	UpdateAnimation();
}

void Player::SetAnimationState(AnimationState state) {
	if (state != animationState_) {
		prevAnimationState_ = animationState_;
		prevTime_           = time_;

		animationT_ = 0.0f;

		animationState_ = state;
		time_           = {};
	}
}

void Player::SetAttributeImGui() {
	if (ImGui::TreeNode("collider")) {
		hitCollider_->SetImGuiCommand();
		ImGui::TreePop();
	}
}

void Player::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	SxavengerModule::GetSpriteCommon()->DrawSprite(
		{}, kMainWindowSize, texture_->GetGPUHandleSRV()
	);
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
	if (animationT_ < 1.0f) {
		animationT_ += SxavengerSystem::GetDeltaTime().time / 0.2f;
		animationT_ = std::clamp(animationT_, 0.0f, 1.0f);

		skeleton_->UpdateTransitionAnimation(
			animators_[prevAnimationState_]->GetAnimation(0), prevTime_, true,
			animators_[animationState_]->GetAnimation(0), time_, true,
			animationT_
		);

	} else {
		skeleton_->UpdateAnimation(animators_[animationState_]->GetAnimation(0), time_);
	}
}

void Player::UpdateCamera() {
	pivot_ = GetPosition();
	pivotRotation_.x = std::fmod(pivotRotation_.x, pi_v * 2.0f);
	pivotRotation_.y = std::clamp(pivotRotation_.y, 0.0f, pi_v / 14.0f);

	Quaternion rotate
		= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pivotRotation_.x)
		* MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, pivotRotation_.y);

	static const Vector3f direction = { 0.0f, 0.0f, -1.0f };
	Vector3f rotatedDirection = RotateVector(direction, rotate);

	camera_->GetTransform().translate = pivot_ + rotatedDirection * distance_ + offset_;
	camera_->GetTransform().rotate    = rotate;
	camera_->UpdateMatrix();
}
