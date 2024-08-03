#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// lib
#include <Camera3D.h>

// Input
#include <Input.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugCamera3D class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugCamera3D
	: public Camera3D {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	/*DebugCamera3D() = default;

	~DebugCamera3D() { Term(); }*/

	void Update();

	//* attribute *//

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Input* input_ = Input::GetInstance();

	//* parameter *//

	float deltaMove_     = 0.01f;
	float deltaRotate_   = 0.01f;
	float deltaDistance_ = 0.5f;

	//* member *//

	Vector3f pivot_ = {};
	float lon_, lat_;
	float distance_ = 10.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CalculateView();

};