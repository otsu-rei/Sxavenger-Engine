#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "IScene.h"



////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class
////////////////////////////////////////////////////////////////////////////////////////////
class Scene_Title
	: public IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Scene_Title()           = default;
	~Scene_Title() override { Term(); }

	void Init() override;

	void Term() override;

	void Update() override;

	void Draw() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	

};