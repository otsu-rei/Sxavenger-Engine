#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////
void Player::Init() {

	//* external *//
	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	//* state *//
	state_ = std::make_unique<PlayerStateRoot>(this);
	state_->Init();

	ModelBehavior::SetName("player");
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	ModelBehavior::model_ = SxavengerAsset::Import<Model>("asset/model/demo/cube.obj");
	SxavengerAsset::PushTask(ModelBehavior::model_.value().Lock());

}

void Player::Term() {
}

void Player::Update() {
	UpdateState();

	ModelBehavior::UpdateMatrix();
}

void Player::UpdateState() {
	//!< apply request state
	if (requestState_.has_value()) {
		state_->Term(); //!< 前のstateを終了

		state_ = std::move(requestState_.value()); //!< request stateを適用
		state_->Init();
	}

	//!< update state
	state_->Update();
}
