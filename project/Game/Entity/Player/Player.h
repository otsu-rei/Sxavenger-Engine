#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player 
#include "BasePlayerState.h"
#include "PlayerStateRoot.h"

//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Behavior/AnimationBehavior.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public AnimationBehavior {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Animation enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum AnimationState : uint8_t {
		Idle,
		Walking,
	};
	static const uint8_t kAnimationCount = AnimationState::Walking + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player()  = default;
	~Player() = default;

	void Init();

	void Term();

	void Update();

	void SetAnimationState();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput*  gamepad_  = nullptr;

	//* state *//

	std::unique_ptr<BasePlayerState>                state_        = nullptr;
	std::optional<std::unique_ptr<BasePlayerState>> requestState_ = std::nullopt;

	//* animation *//

	std::array<std::shared_ptr<Animator>, kAnimationCount> animators_;
	AnimationState animationState_ = AnimationState::Idle;

	TimePointf<TimeUnit::second> time_;

	std::unique_ptr<SkeletonMesh> skeleton_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateState();

	void UpdateAnimation();
	
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// friend class
	////////////////////////////////////////////////////////////////////////////////////////////
	friend PlayerStateRoot;
};
