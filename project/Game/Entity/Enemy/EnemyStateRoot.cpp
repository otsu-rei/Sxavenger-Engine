#include "EnemyStateRoot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateRoot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateRoot::Init() {
}

void EnemyStateRoot::Term() {
}

void EnemyStateRoot::Update() {
	enemy_->time_ += SxavengerSystem::GetDeltaTime();
	enemy_->SetAnimationState(Enemy::AnimationState::FightingIdle);
}
