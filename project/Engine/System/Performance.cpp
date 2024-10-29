#include "Performance.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Console/SystemConsole.h>

//* c++
#include <thread>

//=========================================================================================
// static variables
//=========================================================================================
//* chrono *//
std::chrono::steady_clock::time_point Performance::reference_;

//* chrono *//
float Performance::deltaTime_ = 0.0f;

//* buffer *//
std::unique_ptr<BufferResource<Performance::PerFrame>> Performance::perFrameBuffer_;

float Performance::secondsConversions_[SecondsUnit::kCountOfSecondsUnit] = {
	1.0f, 0.001f, 0.000001f
};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::BeginFrame() {
	reference_ = std::chrono::steady_clock::now();
}

void Performance::EndFrame() {

	WaitForFPS(120);

	if (!sSystemConsole->IsUpdateRequired()) {
		return;
	}

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// deltaTimeの書き込み
	deltaTime_ = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - reference_).count());

	if (perFrameBuffer_ != nullptr) { //!< bufferが存在する場合
		(*perFrameBuffer_)[0].deltaTime = GetDeltaTime(s).time; //!< 秒でのdeltaTimeをBufferに書き込み
	}
}

DeltaTimePoint Performance::GetDeltaTime(SecondsUnit unit) {

	DeltaTimePoint result = {};

	if (unit == kCountOfSecondsUnit) {
		result.time = 0.0f;
		return result; //!< 例外処理
	}

	result.time = deltaTime_ * secondsConversions_[unit];
	return result;
}

void Performance::CreateBuffer() {
	perFrameBuffer_ = std::make_unique<BufferResource<PerFrame>>(Sxavenger::GetDevicesObj(), 1);

	// 現在の時間をBufferに書き込み
	auto now = std::chrono::high_resolution_clock::now();
	(*perFrameBuffer_)[0].time = std::chrono::duration<float>(now.time_since_epoch()).count();
}

void Performance::TermBuffer() {
	perFrameBuffer_.reset();
}

const D3D12_GPU_VIRTUAL_ADDRESS Performance::GetGPUVirtualAddress() {
	Assert(perFrameBuffer_ != nullptr); //!< bufferが存在しない場合, 値を返せない
	return perFrameBuffer_->GetGPUVirtualAddress();
}

void Performance::WaitForFPS(float fps) {
	const std::chrono::microseconds kWaitTime(static_cast<uint64_t>(1000000.0f / fps));
	const std::chrono::microseconds kCheckWaitTime(static_cast<uint64_t>(1000000.0f / (fps + 5))); //!< 60Hz倍数以外のモニター対策

	// 現在時刻の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 経過所間の取得
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// frameが指定されたfpsを通過してない場合
	if (elapsed < kCheckWaitTime) {
		while (std::chrono::steady_clock::now() - reference_ < kWaitTime) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}