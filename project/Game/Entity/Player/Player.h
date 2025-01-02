#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player 
#include "BasePlayerState.h"
#include "PlayerStateRoot.h"

//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Module/Behavior/ModelBehavior.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player()  = default;
	~Player() = default;

	void Init();

	void Term();

	void Update();

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

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateState();
	
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// friend class
	////////////////////////////////////////////////////////////////////////////////////////////
	friend PlayerStateRoot;
};
