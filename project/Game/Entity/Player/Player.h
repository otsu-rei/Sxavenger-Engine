#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player 
#include "BasePlayerState.h"
#include "PlayerStateRoot.h"
#include "PlayerStateRolling.h"
#include "PlayerStatePunch.h"
#include "PlayerStateHook.h"
#include "PlayerStateElbow.h"
#include "PlayerStateStraight.h"

//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Behavior/AnimationBehavior.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>
#include <Engine/Module/Collider/Collider.h>

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
		Running,
		Rolling,
		Punching,
		Hooking,
		Elbow,
		Straight
	};
	static const uint8_t kAnimationCount = AnimationState::Straight + 1;

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

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput*  gamepad_  = nullptr;

	Camera3d* camera_ = nullptr;

	//* state *//

	std::unique_ptr<BasePlayerState>                state_        = nullptr;
	std::optional<std::unique_ptr<BasePlayerState>> requestState_ = std::nullopt;

	//* animation *//

	std::array<std::shared_ptr<Animator>, kAnimationCount> animators_;
	AnimationState animationState_ = AnimationState::Idle;

	TimePointf<TimeUnit::second> time_;

	std::unique_ptr<SkeletonMesh> skeleton_;

	//* camera control *//

	Vector3f pivot_ = kOrigin3<float>;
	Vector2f pivotRotation_ = {}; //!< x: lon, y: lat

	float distance_ = 8.0f;

	Vector3f offset_ = { 0.0f, 1.0f, 0.0f };

	Quaternion target_ = Quaternion::Identity();

	//* collider *//

	std::unique_ptr<Collider> hitCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateState();

	void UpdateAnimation();

	void UpdateCamera();
	
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// friend class
	////////////////////////////////////////////////////////////////////////////////////////////
	friend PlayerStateRoot;
	friend PlayerStateRolling;
	friend PlayerStatePunch;
	friend PlayerStateHook;
	friend PlayerStateElbow;
	friend PlayerStateStraight;
};
