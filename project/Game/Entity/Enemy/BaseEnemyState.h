#pragma once

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Enemy;

////////////////////////////////////////////////////////////////////////////////////////////
// Base EnemyState class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseEnemyState(Enemy* enemy) : enemy_(enemy) {}
	virtual ~BaseEnemyState() = default;

	virtual void Init() = 0;

	virtual void Term() = 0;

	virtual void Update() = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Enemy* enemy_ = nullptr;

};
