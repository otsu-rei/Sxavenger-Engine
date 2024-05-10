#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <chrono>

////////////////////////////////////////////////////////////////////////////////////////////
// SecondsUnit enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SecondsUnit {
	us, //!< マイクロ秒
	ms, //!< ミリ秒
	s,  //!< 秒

	kCountOfSecondsUnit //!< 単位の数
};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance {
public:
	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief フレーム計測開始
	static void BeginFrame();

	//! @breif フレーム計測終了
	static void EndFrame();

	//! @brief 1フレームの時間を返却
	//! @brief ...前1フレームの計測した数値を返却
	//! 
	//! @param[in] uint 秒単位
	static float GetFramesPerformance(SecondsUnit unit) {
		if (unit == kCountOfSecondsUnit) {
			return 0.0f; //!< 例外処理
		}

		return framesPerMicroSec_ * secondsConversions_[unit];
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::chrono::system_clock::time_point startFrame_, endFrame_;
	static float framesPerMicroSec_;

	static float secondsConversions_[SecondsUnit::kCountOfSecondsUnit]; //!< 秒数単位変換表 micro * this[...]

};