#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"
#include "EnemyStateRoot.h"
#include "EnemyStateReactionLight.h"
#include "EnemyStateReactionHeavy.h"

//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Behavior/AnimationBehavior.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>
#include <Engine/Module/Collider/Collider.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy
	: public AnimationBehavior {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Animation enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum AnimationState : uint8_t {
		FightingIdle,
		
		ReactionLight,
		ReactionHeavy,
	};
	static const uint8_t kAnimationCount = AnimationState::ReactionHeavy + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Enemy()  = default;
	~Enemy() = default;

	void Init();

	void Update();

	void SetAnimationState(AnimationState state);

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const target);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::shared_ptr<Animator>, kAnimationCount> animators_;

	AnimationState animationState_ = AnimationState::FightingIdle;
	TimePointf<TimeUnit::second> time_ = {};

	AnimationState prevAnimationState_ = AnimationState::FightingIdle;
	TimePointf<TimeUnit::second> prevTime_ = {};

	float animationT_ = 1.0f;

	std::unique_ptr<SkeletonMesh> skeleton_;

	//* state *//

	std::unique_ptr<BaseEnemyState>                state_        = nullptr;
	std::optional<std::unique_ptr<BaseEnemyState>> requestState_ = std::nullopt;

	//* collider *//

	std::unique_ptr<Collider> hitCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateAnimation();

	void UpdateState();

public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// friend
	////////////////////////////////////////////////////////////////////////////////////////////
	friend EnemyStateRoot;
	friend EnemyStateReactionLight;
	friend EnemyStateReactionHeavy;
};
