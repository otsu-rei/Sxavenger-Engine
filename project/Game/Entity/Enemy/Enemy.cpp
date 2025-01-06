#include "Enemy.h"

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

	animators_[AnimationState::FightingIdle] = SxavengerAsset::TryImportPtr<Animator>("asset/model/sample/fighting_idle.gltf").lock();

	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { SxavengerAsset::PushTask(animator); });
	std::for_each(animators_.begin(), animators_.end(), [](auto& animator) { animator->WaitComplete(); });

	animationState_ = AnimationState::FightingIdle;

}

void Enemy::Update() {
	AnimationBehavior::UpdateMatrix();

	//* update skeleton *//

	UpdateAnimation();
}

void Enemy::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();
	skeleton_->UpdateAnimation(animators_[animationState_]->GetAnimation(0), time_);
}
