#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BasePlayerState.h"

//* engine
#include <Engine/System/Runtime/Input/Input.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRoot class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateRoot
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateRoot(Player* player) : BasePlayerState(player) {}
	~PlayerStateRoot() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* paraemter *//

	float speed_ = 1.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void MoveGamepad();
	void MoveKeyboard();

	// HACK: 移動を扱う
};
