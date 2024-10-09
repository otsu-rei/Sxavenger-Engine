#include "DebugCamera3D.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <algorithm>
#include <Lib/Geometry/Quaternion.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugCamera3D class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera3D::Update() {
	//* blenderのカメラの移動方法を使用
	
	if (input_->IsPressMouse(2)) { //!< mouseの中ボタンが押されたとき

		// mouseの移動量を取得
		Vector2i delta = input_->GetDeltaMousePos();

		if (input_->IsPressKey(DIK_LSHIFT)) {

			Vector2f move = static_cast<Vector2f>(delta) * deltaMove_;

			// 現在のカメラ基準でのrotateからmouseの移動量分を加算
			pivot_ += Matrix::Transform({ -move.x, move.y, 0.0f }, Matrix::MakeRotate(transform_.transform.rotate));
			// HACK: move.xをマイナスしなければいけない原因を解明

		} else {

			// mouseの移動量分の回転
			lon_ += delta.x * deltaRotate_;
			lat_ += delta.y * deltaRotate_;
		}
	}

	// pivotとの距離
	distance_ += deltaDistance_ * input_->GetDeltaWheel() * -1;
	// HACK: mouseWheelを反転する原因の解明

	// pivotの奥に行かない用に調整
	distance_ = (std::max)(0.0f, distance_);

	CalculateView();

}

void DebugCamera3D::Reset() {
	pivot_ = {};
	lon_ = 0.0f;
	lat_ = 0.0f;
	distance_ = 10.0f;

	CalculateView();
}

void DebugCamera3D::CalculateView() {

	// translate
	Vector3f point = {
		std::cos(lat_) * -std::sin(lon_),
		std::sin(lat_),
		std::cos(lat_) * -std::cos(lon_),
	};

	point *= distance_;

	transform_.transform.translate = point + pivot_;

	// rotate
	transform_.transform.rotate.x = lat_;
	transform_.transform.rotate.y = lon_;

	Camera3D::UpdateTranslate();
	
}
