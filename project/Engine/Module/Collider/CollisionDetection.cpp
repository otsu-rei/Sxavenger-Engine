#include "CollisionDetection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool CollisionDetection::CheckCollision(
	const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
	const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB) {

	Visitor visitor = {};
	visitor.positionA = positionA;
	visitor.positionB = positionB;

	return std::visit(visitor, boundingA, boundingB);
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

bool CollisionDetection::SphereToCapsule(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	Vector3f origin = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diff   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - origin;

	float lengthSq = Dot(diff, diff);
	float t = 0.0f;

	if (lengthSq != 0.0f) {
		t = Dot(diff, positionA - origin) / lengthSq;
	}

	t = std::clamp(t, 0.0f, 1.0f);

	Vector3f closest = origin + t * diff;

	float distance = Length(closest - positionA);

	if (distance <= sphereA.radius + capsuleB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToAABB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {


	Vector3f closestPoint = Clamp(positionA, aabbB.min + positionB, aabbB.max + positionB);

	float distance = Length(closestPoint - positionA);

	if (distance <= sphereA.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToOBB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f centerAInOBBLocal = Matrix::Transform(positionA, Matrix4x4(obbB.orientation * Matrix::MakeTranslate(positionB)).Inverse());

	CollisionBoundings::Sphere sphereAInOBBLocal = {
		.radius = sphereA.radius
	};

	CollisionBoundings::AABB aabbBInOBBLocal = {
		.min = -obbB.size,
		.max = obbB.size,
	};

	return SphereToAABB(
		centerAInOBBLocal, sphereAInOBBLocal,
		{}, aabbBInOBBLocal
	);
}

bool CollisionDetection::CapsuleTo(
	const Vector3f& positionA, const CollisionBoundings::Capsule& capsuleA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	// 線分の生成
	Vector3f originA = positionA - capsuleA.direction * (capsuleA.length * 0.5f);
	Vector3f diffA   = positionA + capsuleA.direction * (capsuleA.length * 0.5f) - originA;
	Vector3f originB = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diffB   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - originB;

	Vector3f r = originA - originB;

	float A = Dot(diffA, diffA); // a.diff の長さの2乗
	float B = Dot(diffA, diffB); // a.diff と b.diff の内積
	float C = Dot(diffB, diffB); // b.diff の長さの2乗
	float D = Dot(diffA, r);     // a.diff と r の内積
	float E = Dot(diffB, r);     // b.diff と r の内積

	float denominator = A * C - B * B;

	float distance = {};

	if (denominator != 0.0f) {
		// s と t を計算
		float s, t = {};

		s = (B * E - C * D) / denominator;
		t = (A * E - B * D) / denominator;

		// 範囲制約を適用
		s = std::clamp(s, 0.0f, 1.0f);
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3f closestA = originA + s * originA;
		Vector3f closestB = originB + t * originB;

		distance = Length(closestA - closestB);

	} else {
		// HACK
		Vector3f endA = originA + diffA;
		Vector3f endB = originB + diffB;

		distance = Length(originA - originB);
		distance = std::min(Length(originA - endB), distance);
		distance = std::min(Length(endA - originB), distance);
		distance = std::min(Length(endA - endB), distance);
	}

	if (distance <= capsuleA.radius + capsuleB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBTo(
	const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {


	if (All(aabbA.min + positionA <= aabbB.max + positionB)
		&& All(aabbA.max + positionA >= aabbB.min + positionB)) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBToOBB(
	const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f axes[3 + 3 + 9] = {};

	Vector3f aabbAxesA[3] = {};
	Vector3f obbAxesB[3] = {};

	aabbAxesA[0] = { 1.0f, 0.0f, 0.0f };
	aabbAxesA[1] = { 0.0f, 1.0f, 0.0f };
	aabbAxesA[2] = { 0.0f, 0.0f, 1.0f };

	obbAxesB[0] = Matrix::Transform({ 1.0f, 0.0f, 0.0f }, obbB.orientation);
	obbAxesB[1] = Matrix::Transform({ 0.0f, 1.0f, 0.0f }, obbB.orientation);
	obbAxesB[2] = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, obbB.orientation);

	int k = 0;

	axes[k++] = aabbAxesA[0];
	axes[k++] = aabbAxesA[1];
	axes[k++] = aabbAxesA[2];

	axes[k++] = obbAxesB[0];
	axes[k++] = obbAxesB[1];
	axes[k++] = obbAxesB[2];

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[k++] = Cross(aabbAxesA[i], obbAxesB[j]);
		}
	}

	for (int i = 0; i < 15; ++i) {
		Vector3f axisNormalize = Normalize(axes[i]);

		float aabbAProjection
			= std::abs((aabbA.max.x - aabbA.min.x) * 0.5f * Dot(aabbAxesA[0], axisNormalize))
			+ std::abs((aabbA.max.y - aabbA.min.y) * 0.5f * Dot(aabbAxesA[1], axisNormalize))
			+ std::abs((aabbA.max.z - aabbA.min.z) * 0.5f * Dot(aabbAxesA[2], axisNormalize));


		float obbBProjection
			= std::abs(obbB.size.x * Dot(obbAxesB[0], axisNormalize))
			+ std::abs(obbB.size.y * Dot(obbAxesB[1], axisNormalize))
			+ std::abs(obbB.size.z * Dot(obbAxesB[2], axisNormalize));

		float disatnce = std::abs(Dot(positionA - positionB, axisNormalize));

		if (disatnce > aabbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}

bool CollisionDetection::OBBTo(
	const Vector3f& positionA, const CollisionBoundings::OBB& obbA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f axes[3 + 3 + 9] = {};

	Vector3f obbAxesA[3] = {};
	Vector3f obbAxesB[3] = {};

	obbAxesA[0] = Matrix::Transform({ 1.0f, 0.0f, 0.0f }, obbA.orientation);
	obbAxesA[1] = Matrix::Transform({ 0.0f, 1.0f, 0.0f }, obbA.orientation);
	obbAxesA[2] = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, obbA.orientation);

	obbAxesB[0] = Matrix::Transform({ 1.0f, 0.0f, 0.0f }, obbB.orientation);
	obbAxesB[1] = Matrix::Transform({ 0.0f, 1.0f, 0.0f }, obbB.orientation);
	obbAxesB[2] = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, obbB.orientation);

	int k = 0;

	axes[k++] = obbAxesA[0];
	axes[k++] = obbAxesA[1];
	axes[k++] = obbAxesA[2];

	axes[k++] = obbAxesB[0];
	axes[k++] = obbAxesB[1];
	axes[k++] = obbAxesB[2];

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[k++] = Cross(obbAxesA[i], obbAxesB[j]);
		}
	}

	for (int i = 0; i < 15; ++i) {

		Vector3f axisNormalize = Normalize(axes[i]);

		float obbAProjection
			= std::abs(obbA.size.x * Dot(obbAxesA[0], axisNormalize))
			+ std::abs(obbA.size.y * Dot(obbAxesA[1], axisNormalize))
			+ std::abs(obbA.size.z * Dot(obbAxesA[2], axisNormalize));

		float obbBProjection
			= std::abs(obbB.size.x * Dot(obbAxesB[0], axisNormalize))
			+ std::abs(obbB.size.y * Dot(obbAxesB[1], axisNormalize))
			+ std::abs(obbB.size.z * Dot(obbAxesB[2], axisNormalize));

		float disatnce = std::abs(Dot(positionA - positionB, axisNormalize));

		if (disatnce > obbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}

bool CollisionDetection::ExceptionUnimplement() {
	Assert(false, "unimplemented."); //!< 未実装
	return false;
}
