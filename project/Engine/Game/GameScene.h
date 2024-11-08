#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <memory>

//* Game
#include <Game/Player.h>
#include <Game/Enemy.h>
#include <Game/AtmosphericScattering.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Run();

private:

	//* sample game
	//* 

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unique_ptr<Player> player_;

	std::unique_ptr<EnemyCollection> enemyCollection_;

	std::unique_ptr<AtmosphericScattering> atmosphericScattering_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};