#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Bunny class
////////////////////////////////////////////////////////////////////////////////////////////
class Bunny
	: public Object {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Bunny() { Init(); }

	//! @brief デストラクタ
	~Bunny() { Term(); }

	//! @brief 初期化処理
	void Init() override;

	//! @brief ImGui
	void EditorImGui(int identifier);

	//! @brief 描画処理
	//! 
	//! @param[in] commandList
	void Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) override;

	//! @brief 終了処理
	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Model> model_;

};