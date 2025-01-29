#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Adapter/Scene/BaseScene.h>

//* engine
#include <Engine/!Render/Scene/Actor/Light/LightActors/ADirectionalLightActor.h>


//* game
//#include "../Entity/Player/Player.h"
//#include "../Entity/Enemy/Enemy.h"
//#include "../Object/Ground.h"

//* game
#include "../Object/GameCamera.h"
#include "../Entity/Player/Player.h"
#include "../Object/Ground.h"

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneGame
	: public BaseScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SceneGame() = default;
	~SceneGame() = default;

	void Init() override;

	void Update() override;

	void Draw() override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<GameCamera> camera_;

	std::unique_ptr<Player> player_;

	/*std::unique_ptr<Enemy> enemy_;*/

	std::unique_ptr<Ground> ground_;



	std::unique_ptr<ADirectionalLightActor> light_;

};
