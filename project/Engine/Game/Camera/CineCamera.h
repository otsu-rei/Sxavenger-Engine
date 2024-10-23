#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Camera/Camera3D.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CineCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class CineCamera
	: public Camera3D {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CineCamera() { Init(); }

	~CineCamera() { Term(); }

	void Init();

	void Term();

	//* setter *//

	void SetProjection(const Vector2f& sensorSize, float focalLength, float nearClip, float farClip);

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* cinema parameter *//

	Vector2f sensorSize_;  /*_mm*/
	float    focalLength_; /*_mm*/

	float nearClip_;
	float farClip_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CalculateProjection();


};