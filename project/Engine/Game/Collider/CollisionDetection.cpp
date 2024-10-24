#include "CollisionDetection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cmath>
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool CollisionDetection::CheckCollision(
	const Vector3f& posisionA, const CollisionBoundings::Boundings& boundingA,
	const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB) {

	 // boundingA の種類に応じて処理を分岐
	if (auto* sphereA = std::get_if<CollisionBoundings::Sphere>(&boundingA)) {
		return HandleSphereCollision(posisionA, *sphereA, posisionB, boundingB);

	} else if (auto* aabbA = std::get_if<CollisionBoundings::AABB>(&boundingA)) {
		return HandleAABBCollision(posisionA, *aabbA, posisionB, boundingB);

	} else if (auto* obbA = std::get_if<CollisionBoundings::OBB>(&boundingA)) {
		return HandleOBBCollision(posisionA, *obbA, posisionB, boundingB); // OBBの処理は未実装
	}

	Assert(false); // 無効な型
	return false;
}

bool CollisionDetection::HandleSphereCollision(
	const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB) {

	if (auto* sphereB = std::get_if<CollisionBoundings::Sphere>(&boundingB)) {
		return SphereTo(posisionA, sphereA, posisionB, *sphereB);

	} else if (auto* aabbB = std::get_if<CollisionBoundings::AABB>(&boundingB)) {
		return SphereToAABB(posisionA, sphereA, posisionB, *aabbB);

	} else if (auto* obbB = std::get_if<CollisionBoundings::OBB>(&boundingB)) {
		// OBBとの当たり判定は未実装
		return false;
	}
		 
	Assert(false); //!< 無効な型
	return false;
}

bool CollisionDetection::HandleAABBCollision(
	const Vector3f& posisionA, const CollisionBoundings::AABB&      aabbA,
	const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB) {


	if (auto* sphereB = std::get_if<CollisionBoundings::Sphere>(&boundingB)) {
		return SphereToAABB(posisionB, *sphereB, posisionA, aabbA);

	} else if (auto* aabbB = std::get_if<CollisionBoundings::AABB>(&boundingB)) {
		return AABBTo(posisionA, aabbA, posisionB, *aabbB);

	} else if (auto* obbB = std::get_if<CollisionBoundings::OBB>(&boundingB)) {
		// OBBとの当たり判定は未実装
		return false;
	}

	Assert(false); //!< 無効な型
	return false;
}

bool CollisionDetection::HandleOBBCollision(
	const Vector3f& posisionA, const CollisionBoundings::OBB& obbA,
	const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB) {

	posisionA, obbA, posisionB, boundingB;

	return false; //!< 未実装
}

bool CollisionDetection::SphereTo(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB) {

	float distance = Length(positionA - positionB);

	if (distance <= sphereA.radius + sphereB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToAABB(
	const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB) {

	Vector3f closestPoint = Clamp(posisionA, aabbB.localMin + posisionB, aabbB.localMax + posisionB);

	float distance = Length(closestPoint - posisionA);

	if (distance <= sphereA.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBTo(
	const Vector3f& posisionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB) {

	if (All(aabbA.localMin + posisionA <= aabbB.localMax + posisionB) 
		&& All(aabbA.localMax + posisionA >= aabbB.localMin + posisionB)) {

		return true;
	}

	return false;
}
