#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BasePlayerState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/Module/Collider/Collider.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateStraight
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateStraight(Player* player) : BasePlayerState(player) {};
	~PlayerStateStraight() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* time *//

	TimePointf<TimeUnit::second> time_;
	TimePointf<TimeUnit::second> duration_;

	//* next state *//

	std::optional<std::unique_ptr<BasePlayerState>> nextAttackState_ = std::nullopt; //!< 次の攻撃状態

	//* collider *//

	std::unique_ptr<Collider> attackCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ActionGamepad();

	void UpdateAnimation();

};
