#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BaseEnemyState.h"
#include "EnemyStateRoot.h"
#include "EnemyStateApproach.h"
#include "EnemyStateStraight.h"
#include "EnemyStateReactionLight.h"
#include "EnemyStateReactionHeavy.h"
#include "EnemyStateKnock.h"

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
		Approach,
		Straight,
		
		ReactionLight,
		ReactionHeavy,

		Knock,
	};
	static const uint8_t kAnimationCount = AnimationState::Knock + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Enemy()  = default;
	~Enemy() = default;

	void Init();
	void Init(const QuaternionTransform& transform);

	void Update();

	void SetAnimationState(AnimationState state);

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const target);

	void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

	void SetTarget(Collider* target) { target_ = target; }

	void SetAttributeImGui() override;

	bool IsDead() const { return isDead_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* visual *//

	std::shared_ptr<Texture> texture_ = nullptr;

	//* animation *//

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

	//* hp *//

	float hp_ = 12.0f;

	//* target *//

	Collider* target_ = nullptr;
	//! targetになるcollider

	//* collider *//

	std::unique_ptr<Collider> hitCollider_;

	bool isDead_ = false;

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
	friend EnemyStateApproach;
	friend EnemyStateStraight;
	friend EnemyStateReactionLight;
	friend EnemyStateReactionHeavy;
	friend EnemyStateKnock;
};
