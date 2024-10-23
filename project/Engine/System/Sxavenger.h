#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* origin
#include "WinApp.h"
#include "DirectXCommon.h"
//#include <DirectXRCommon.h>
#include "ImGuiManager.h"
#include "Texture.h"
#include "Input.h"
#include "Audio.h"

//* beta
#include <Engine/Beta/DepthRenderTarget.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerEngine class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerEngine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	static void Init(const Vector2ui& windowSize, const char* windowTitle);

	//! @brief 終了処理
	static void Term();

	//-----------------------------------------------------------------------------------------
	// frame option
	//-----------------------------------------------------------------------------------------

	//! @brief フレームの開始
	static void BeginFrame();

	//! @brief フレームの終了
	static void EndFrame();

	//! @brief プロセスメッセージを取得
	//! @retval 1 ゲーム終了
	//! @retval 0 ゲーム継続
	static int ProcessMessage();

	//-----------------------------------------------------------------------------------------
	// draw canvas option
	//-----------------------------------------------------------------------------------------

	//! @brief スクリーン描画処理の開始
	static void BeginScreenDraw();
	//!< EndはEndFrameに入ってる
	
	//! @param[in] offscreenRenderTexture 書き込む用のdummyTexture
	static void BeginOffscreen(RenderTexture* renderTexture);

	//! @brief オフスク描画処理の開始
	static void EndOffscreen(RenderTexture* renderTexture);

	static void BeginOffscreen(DepthRenderTarget* depthRenderTarget, bool isClearRenderTarget = false);

	static void EndOffscreen(DepthRenderTarget* depthRenderTarget);

	//-----------------------------------------------------------------------------------------
	// original system option
	//-----------------------------------------------------------------------------------------

	static void TranstionAllocator();

	static void ResetBackAllocator();

	//-----------------------------------------------------------------------------------------
	// DirectX12, _DXOBJECT option
	//-----------------------------------------------------------------------------------------

	static _DXOBJECT Descriptor GetDescriptor(_DXOBJECT DescriptorType type);

	static void DeleteDescriptor(_DXOBJECT Descriptor& descriptor);

	static ID3D12Device8* GetDevice();
	
	static ID3D12GraphicsCommandList6* GetCommandList();
	// FIXME: あんましたくない

	static _DXOBJECT Devices* GetDevicesObj();

	static DirectXCommon* GetDxCommon();

	//-----------------------------------------------------------------------------------------
	// texture option
	//-----------------------------------------------------------------------------------------

	static RenderTexture* CreateRenderTexture(const std::string& key, const Vector2ui& textureSize, const Color4f& clearColor = kDefaultClearColor);

	static Texture* LoadTexture(const std::string& filePath);

	static void ReleaseTexture(const std::string& key);

	template <DerivedFormBaseTexture T>
	static T* GetTexture(const std::string& textureKey);

	static BaseTexture* GetBaseTexture(const std::string& textureKey);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandleGPU(const std::string& textureKey);

	static TextureManager* GetTextureManager();

	//-----------------------------------------------------------------------------------------
	// Input option
	//-----------------------------------------------------------------------------------------

	static bool IsPressKey(uint8_t dik);

	static bool IsTriggerKey(uint8_t dik);

	static bool IsReleaseKey(uint8_t dik);

	static Input* GetInput();

	//-----------------------------------------------------------------------------------------
	// Audio option
	//-----------------------------------------------------------------------------------------

	static std::unique_ptr<Audio> GetAudio(const std::string& filename, bool isLoop = false);

	static void PlayAudioOneShot(const std::string& filename, float volume = 1.0f);

	static AudioManager* GetAudioManager();

	//-----------------------------------------------------------------------------------------
	// imgui manager option
	//-----------------------------------------------------------------------------------------

	static void EndImGuiAndDrawCall();

private:
	static_assert(_HAS_CXX20, "sxavenger engine enviornment must c++20 over.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Sxavenger = SxavengerEngine;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerEngine class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<DerivedFormBaseTexture T>
inline T* SxavengerEngine::GetTexture(const std::string& textureKey) {
	return GetTextureManager()->GetTexture<T>(textureKey);
}
