#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>

//* lib
#include <Lib/MyMath.h>

//* game
#include "../Player/PlayerAttackCollider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init() {

	AnimationBehavior::SetName("enemy");
	AnimationBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	//AnimationBehavior::model_ = SxavengerAsset::TryImport<Model>("asset/model/sample/fighting_idle.gltf");
	AnimationBehavior::model_ = SxavengerAsset::TryImport<Model>("asset/model/sample/idle.gltf");
	SxavengerAsset::PushTask(AnimationBehavior::model_.value().Lock());
	AnimationBehavior::model_.value().Lock()->WaitComplete();

	//* visual *//

	texture_ = SxavengerAsset::TryImport<Texture>("asset/textures/pattern_camo.jpg").Lock();
	SxavengerAsset::PushTask(texture_);
	texture_->WaitComplete();


	//* animation *//

	skeleton_ = std::make_unique<SkeletonMesh>();
	skeleton_->Create(model_.value().Lock());
	AnimationBehavior::skeletonMesh_ = skeleton_.get();

	animators_[AnimationState::FightingIdle]   = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/fighting_idle.gltf").lock();
	animators_[AnimationState::Approach]       = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/approach.gltf").lock();
	animators_[AnimationState::Straight]       = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/straight.gltf").lock();
	animators_[AnimationState::ReactionLight]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/reaction_light.gltf").lock();
	animators_[AnimationState::ReactionHeavy]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/reaction_heavy.gltf").lock();
	animators_[AnimationState::Knock]          = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/knock.gltf").lock();

	std::for_each(
		animators_.begin(), animators_.end(), [](auto& animator) {
			SxavengerAsset::PushTask(animator);
			animator->WaitComplete();
		}
	);
	//std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { animator->WaitComplete(); });

	//* state *//

	state_ = std::make_unique<EnemyStateRoot>(this);
	state_->Init(); 

	//* collider *//

	hitCollider_ = std::make_unique<Collider>();
	hitCollider_->SetToCollection();

	hitCollider_->SetColliderBoundingCapsule({
		{0.0f, 1.0f, 0.0f}, //!< direction
		0.3f, 1.0f //!< radius, height
	});

	hitCollider_->SetParent(this);
	hitCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.0f };
	hitCollider_->SetTypeId(ColliderType::kEnemy);
	hitCollider_->SetTargetId(ColliderType::kPlayerAttack);
	hitCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const target) {
			OnCollisionEnter(target);
		}
	);

}

void Enemy::Init(const QuaternionTransform& transform) {
	Init();

	AnimationBehavior::GetTransform() = transform;
	AnimationBehavior::UpdateMatrix();
}

void Enemy::Update() {
	UpdateState();

	AnimationBehavior::UpdateMatrix();
	hitCollider_->UpdateMatrix();

	//* update skeleton *//

	UpdateAnimation();
}

void Enemy::SetAnimationState(AnimationState state) {
	if (state == animationState_) {
		return;
	}

	animationT_         = 0.0f;
	prevAnimationState_ = animationState_;
	prevTime_           = time_;
	animationState_     = state;
	time_               = {};
}

void Enemy::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	if (other->CheckTypeId(ColliderType::kPlayerAttack)) {
		PlayerAttackCollider* attackCollider = dynamic_cast<PlayerAttackCollider*>(other);

		hp_ -= attackCollider->damage_;

		if (hp_ < 0.0f) { //!< dead!
			requestState_ = std::make_unique<EnemyStateKnock>(this);

		} else if (attackCollider->strength_ == AttackStrength::Light) {
			requestState_ = std::make_unique<EnemyStateReactionLight>(this);

		} else {
			requestState_ = std::make_unique<EnemyStateReactionHeavy>(this);
		}

		Vector3f position      = { GetPosition().x, 1.0f, GetPosition().z };
		Vector3f otherPosition = { other->GetPosition().x, 1.0f, other->GetPosition().z };

		AnimationBehavior::GetTransform().rotate
			= ToQuaternion(CalculateEuler(Normalize(otherPosition - position)));
	}
}

void Enemy::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (skeletonMesh_ == nullptr || !model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera", frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform", TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor", MaterialComponent::GetColorGPUVirtualAddress());
	bind.SetHandle("gAlbedo", texture_->GetGPUHandleSRV());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		skeletonMesh_->SetIABuffer(i);

		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		skeletonMesh_->DrawCall(i);
	}
}

void Enemy::SetAttributeImGui() {

	if (ImGui::Button("root")) {
		requestState_ = std::make_unique<EnemyStateRoot>(this);
		target_ = nullptr;
	}
}

void Enemy::UpdateAnimation() {
	if (animationT_ < 1.0f) {
		animationT_ += SxavengerSystem::GetDeltaTime().time / 0.2f;

		skeleton_->UpdateTransitionAnimation(
			animators_[prevAnimationState_]->GetAnimation(0), prevTime_, true,
			animators_[animationState_]->GetAnimation(0), time_, true,
			animationT_
		);

	} else {
		skeleton_->UpdateAnimation(animators_[animationState_]->GetAnimation(0), time_);
	}
}

void Enemy::UpdateState() {
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
