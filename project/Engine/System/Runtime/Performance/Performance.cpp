#include "Performance.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::Begin() {
	runtime_.Begin();
}

void Performance::End() {
	WaitFrame();
	runtime_.End();

	// todo: fixedDeltaTime_の処理
}

void Performance::SystemDebugGui() {
}

void Performance::WaitFrame() const {
	if (limitFrame_ == 0.0f) {
		return;
	}

	const auto& reference = runtime_.GetReference();

	const std::chrono::microseconds kWaitTime(static_cast<uint64_t>(1000000.0f / limitFrame_));
	const std::chrono::microseconds kCheckWaitTime(static_cast<uint64_t>(1000000.0f / (limitFrame_ + 5))); //!< 60Hz倍数以外のモニター対策

	// 現在時刻の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 経過所間の取得
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference);

	// frameが指定されたfpsを通過してない場合
	if (elapsed < kCheckWaitTime) {
		while (std::chrono::steady_clock::now() - reference < kWaitTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

}
