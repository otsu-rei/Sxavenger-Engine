#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>

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

	skeleton_ = std::make_unique<SkeletonMesh>();
	skeleton_->Create(model_.value().Lock());
	AnimationBehavior::skeletonMesh_ = skeleton_.get();

	//* animation *//

	animators_[AnimationState::FightingIdle]   = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/fighting_idle.gltf").lock();
	animators_[AnimationState::ReactionLight]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/reaction_light.gltf").lock();
	animators_[AnimationState::ReactionHeavy]  = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/reaction_heavy.gltf").lock();



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
	//requestState_ = std::make_unique<EnemyStateReactionLight>(this);
	requestState_ = std::make_unique<EnemyStateReactionHeavy>(this);
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
