#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* module
#include "Primitive/DebugPrimitive.h"
#include "Collider/ColliderCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerModule class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerModule {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// primitive option
	//-----------------------------------------------------------------------------------------

	static void ResetPrimtive();

	static void DrawToScene(const DirectXThreadContext* context, const Camera3d* camera);

	static void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void DrawAxis(const Vector3f& center, float length);

	static void DrawGrid(const Vector3f& center, float size);

	static void DrawBox(const Vector3f& min, const Vector3f& max, const Color4f& color);

	static DebugPrimitive* GetDebugPrimitive();

	//-----------------------------------------------------------------------------------------
	// collider collection option
	//-----------------------------------------------------------------------------------------

	static void SetCollider(Collider* collider);

	static void EraseCollider(Collider* collider);

	static void CheckCollision();

	static void DrawCollider();

	static ColliderCollection* GetColliderCollection();

private:
};