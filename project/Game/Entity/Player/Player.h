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
#include "PlayerStateKick.h"

//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Behavior/AnimationBehavior.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>
#include <Engine/Module/Collider/Collider.h>
#include <Engine/Module/VisualLayer/VisualDoF.h>

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
		Kick,
	};
	static const uint8_t kAnimationCount = AnimationState::Kick + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player()  = default;
	~Player() = default;

	void Init();
	void Init(const QuaternionTransform& transform);

	void Term();

	void Update();

	void SetAnimationState(AnimationState state);

	void SetAttributeImGui() override;

	void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

	void SetShake(TimePointf<TimeUnit::second> time, const Vector2f& strength);

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

	AnimationState prevAnimationState_ = AnimationState::Idle;
	TimePointf<TimeUnit::second> prevTime_;

	float animationT_ = 1.0f;

	std::unique_ptr<SkeletonMesh> skeleton_;

	//* camera control *//

	Vector3f pivot_ = kOrigin3<float>;
	Vector2f pivotRotation_ = {}; //!< x: lon, y: lat

	float distance_ = 6.0f;

	Vector3f offset_ = { 0.0f, 1.0f, 0.0f };

	Quaternion target_ = Quaternion::Identity();

	TimePointf<TimeUnit::second> shakeTime_  = {};
	TimePointf<TimeUnit::second> shakeTimer_ = {};

	Vector2f strength_ = {};


	//* collider *//

	std::unique_ptr<Collider> hitCollider_;

	//* visual

	std::unique_ptr<VisualDoF> dof_;

	std::shared_ptr<Texture> texture_ = nullptr;

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
	friend PlayerStateKick;
};
