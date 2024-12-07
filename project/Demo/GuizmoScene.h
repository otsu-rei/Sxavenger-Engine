#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

#include "Engine/Module/Camera/Camera3d.h"

////////////////////////////////////////////////////////////////////////////////////////////
// GuizmoScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GuizmoScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GuizmoScene() = default;
	~GuizmoScene() = default;

	void Run();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> mainWindow_;

	std::unique_ptr<Camera3d> camera_;

	TransformComponent component_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SystemInit();
	void Init();

	void Update();

	void Draw();
	void DrawScreen();

	void Term();

};