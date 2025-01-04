#include "PlayerStateRoot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

//* engine
#include <Engine/System/Runtime/Performance/Performance.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRoot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateRoot::Init() {
}

void PlayerStateRoot::Term() {
}

void PlayerStateRoot::Update() {
	if (player_->gamepad_->IsConnect()) {
		MoveGamepad(); //!< gamepadが接続されている

	} else {
		MoveKeyboard();
	}

	player_->GetTransform().rotate = Slerp(player_->GetTransform().rotate, target_, 0.2f);
}

void PlayerStateRoot::MoveGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	// stickの入力を取得
	Vector2f direction = {};

	static const float deadzone = 0.1f;
	//!< value: 0.0f ~ 1.0f の間のdeadzoneを設定
	// todo: parameter化

	const Vector2f stick = player_->gamepad_->GetStickNormalized(GamepadStickId::STICK_LEFT);

	const float active = std::abs(stick.x) + std::abs(stick.y);

	if (active >= deadzone) { //!< deadzoneと実際の入力値を比較
		direction = stick;
	}

	// 移動
	Vector2f move = direction * speed_;

	player_->GetTransform().translate += Vector3f{ move.x, 0.0f, move.y } * SxavengerSystem::GetDeltaTime().time;

	// animation stateの処理
	if (Any(direction != kOrigin2<float>)) {
		player_->animationState_ = Player::AnimationState::Walking;
		target_ = ToQuaternion(CalculateEuler({ direction.x, 0.0f, direction.y }));

	} else {
		player_->animationState_ = Player::AnimationState::Idle;
	}
}

void PlayerStateRoot::MoveKeyboard() {
	// keyの入力を取得
	Vector2f direction = {};

	if (player_->keyboard_->IsPress(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	// 移動
	Vector2f move = direction * speed_;

	player_->GetTransform().translate += Vector3f(move.x, 0.0f, move.y) * SxavengerSystem::GetDeltaTime().time;

	// animation stateの処理
	if (Any(direction != kOrigin2<float>)) {
		player_->animationState_ = Player::AnimationState::Walking;
		target_ = ToQuaternion(CalculateEuler({direction.x, 0.0f, direction.y}));

	} else {
		player_->animationState_ = Player::AnimationState::Idle;
	}
}

void PlayerStateRoot::UpdateRotation() {

}


