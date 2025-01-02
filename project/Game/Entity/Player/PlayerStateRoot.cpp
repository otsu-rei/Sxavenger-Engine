#include "PlayerStateRoot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRoot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateRoot::Init() {
}

void PlayerStateRoot::Term() {
}

void PlayerStateRoot::Update() {
 	MoveGamepad();
	MoveKeyboard();
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

	player_->GetTransform().translate += Vector3f(move.x, 0.0f, move.y);
}

void PlayerStateRoot::MoveKeyboard() {
}


