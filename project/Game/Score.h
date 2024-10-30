#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/System/Texture.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Score class
////////////////////////////////////////////////////////////////////////////////////////////
class Score
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void SetAttributeImGui() override;

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera);

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t score = 0;
	
private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	Texture* texture_;

	Vector2f margin_ = { 24.0f, 24.0f };

};