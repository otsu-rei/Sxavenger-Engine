#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "Actor/Geometry/AGeometryActor.h"

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>

//* c++
#include <list>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ALightActor;

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
class FScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FScene()  = default;
	~FScene() = default;

	//* actor option *//

	void AddGeometry(AGeometryActor* geometry);

	//* getter *//

	const std::list<AGeometryActor*>& GetGeometries() const { return geometries_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* geometry actors *//

	std::list<AGeometryActor*> geometries_;
	//* sceneに描画されるactorのリスト

	DxrObject::TopLevelAS topLevelAS_;

	//* light actors *//

	std::list<ALightActor*> lights_;
	//* sceneに描画されるactorのリスト

	

};
