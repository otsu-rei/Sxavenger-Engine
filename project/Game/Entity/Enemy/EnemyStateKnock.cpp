#include "EnemyStateKnock.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateKnock class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateKnock::Init() {
	duration_ = enemy_->animators_[Enemy::AnimationState::Knock].WaitGet()->GetDurationTime(0);
	enemy_->SetAnimationState(Enemy::AnimationState::Knock);

	enemy_->hitCollider_->SetIsActive(false);
}

void EnemyStateKnock::Term() {
}

void EnemyStateKnock::Update() {

	time_ += SxavengerSystem::GetDeltaTime().time * enemy_->delta_;

	if (time_ >= duration_) {
		enemy_->isDead_ = true;
	}

	time_.time = std::min(time_.time, { duration_.time - 0.01f });

	enemy_->time_ = time_;

}
