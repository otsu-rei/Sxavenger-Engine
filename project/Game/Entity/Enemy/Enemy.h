#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Behavior/AnimationBehavior.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>

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
	};
	static const uint8_t kAnimationCount = AnimationState::FightingIdle + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Enemy()  = default;
	~Enemy() = default;

	void Init();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::shared_ptr<Animator>, kAnimationCount> animators_;
	AnimationState animationState_ = AnimationState::FightingIdle;

	TimePointf<TimeUnit::second> time_;

	std::unique_ptr<SkeletonMesh> skeleton_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateAnimation();
	
};
