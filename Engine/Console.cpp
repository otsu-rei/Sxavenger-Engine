#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "externals/imgui/imgui.h"
#include "MyEngine.h"
#include "Environment.h"
#include "Performance.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

#include <Attribute.h>
#include <format>

//-----------------------------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================
const Vector4f Console::commentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Vector4f Console::errorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Vector4f Console::warningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };
const Vector4f Console::defaultColor    = { 0.98f, 0.98f, 0.98f, 1.0f };

////////////////////////////////////////////////////////////////////////////////////////////
// Console class methods
////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************************
// 一時的にシングルトンとしている。
//*****************************************************************************************

Console* Console::GetInstance() {
	static Console instance;
	return &instance;
}

void Console::Init() {
	dxCommon_ = MyEngine::GetDxCommon();

	sceneTexture_ = MyEngine::CreateRenderTexture(kWindowWidth, kWindowHeight, "sceneTexture");

	debugCamera_ = std::make_unique<Camera3D>();
	/*debugCamera_->SetAttributeName("debugCamera");*/


}

void Console::Term() {
	logDatas_.clear();
	debugCamera_.reset();
}

void Console::Update() {

	OutputMenu();

	if (isOutputConsole_) {

		windowFlags = ImGuiWindowFlags_NoCollapse;

		if (isFix_) {
			windowFlags |= ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoResize;
		}

		// game window
		OutputScene();
		OutputGame();

		OutputLog();
		OutputOutliner();
		OutputPerformance();
		OutputSystem();
	}
}

void Console::SetLog(const std::string& log, const Vector4f& color) {
	// logの追加
	logDatas_.push_front({log, color});

	while (logDatas_.size() >= kMaxLogData_) {
		// 一番古いログの削除
		logDatas_.pop_back();
	}
}

void Console::CheckEraseAttribute(Attribute* obj) {
	if (obj == selectedAttribute_) { //!< 削除するattributeが今選択されてるattributeだった場合
		selectedAttribute_ = nullptr;
	}
}

void Console::OutputRayTracingResult(const D3D12_GPU_DESCRIPTOR_HANDLE& srvHandleGPU) {
	if (isOutputConsole_) {
		static bool isOpenWindow = true;
		ImGui::Begin("RayTracing", &isOpenWindow, windowFlags);

		SetTextureImGui(srvHandleGPU);

		ImGui::End();
	}
}

void Console::OutputTexture(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& srvHandleGPU) {
	if (isOutputConsole_) {
		static bool isOpenWindow = true;
		ImGui::Begin(name.c_str(), &isOpenWindow, windowFlags);

		SetTextureImGui(srvHandleGPU);

		ImGui::End();
	}
}

//=========================================================================================
// private methods
//=========================================================================================

void Console::OutputMenu() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Settings")) {

			ImGui::Checkbox("DisplayConsole", &isOutputConsole_);

			// windowFlags
			ImGui::Checkbox("Fix(lock size and move)", &isFix_);


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Console::OutputScene() {
	static bool isOpenWindow = true;
	ImGui::Begin("Scene", &isOpenWindow, windowFlags);
	// todo: window開かれてる状態を保存させる. foucusとは違う
	isFocusDebugScene_; //!< 名前もfoucusからいい感じに変更させる

	SetTextureImGui(sceneTexture_->GetSRVHandleGPU());

	ImGui::End();
}

void Console::OutputGame() {
	static bool isOpenWindow = true;
	ImGui::Begin("Game", &isOpenWindow, windowFlags);
	// this->windowがフォーカスされてるかどうか
	isFocusGameScene_ = ImGui::IsWindowFocused();

	SetTextureImGui(MyEngine::GetTextureHandleGPU("offscreen"));

	ImGui::End();
}

void Console::OutputLog() {
	//! 開いておく
	static bool isOpenWindow = true;
	ImGui::Begin("Log", &isOpenWindow, windowFlags);

	if (ImGui::BeginTabBar("LogTab")) {

		if (ImGui::BeginTabItem("Console")) {

			ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
			ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
			ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

			
			ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
			for (auto& log : logDatas_) {
				ImGui::TextColored({ log.color.r, log.color.g, log.color.b, log.color.a }, log.log.c_str());
			}
			ImGui::EndChild();
			

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Printf")) {


			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Console::OutputPerformance() {
	static bool isOpenWindow = true;
	ImGui::Begin("Performance", &isOpenWindow, windowFlags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	float framesPerSec = Performance::GetFramesPerformance(SecondsUnit::s);

	ImGui::Text("exec speed / frame: %.6f", framesPerSec);
	ImGui::SameLine();
	ImGui::Text("FPS: %.1f", 1.0f / framesPerSec);

	ImGui::End();
}

void Console::OutputOutliner() {
	// Inspectorの選択window
	static bool isOpenWindow = true;
	ImGui::Begin("Outliner", &isOpenWindow, windowFlags);

	// TODO: 名前の重複参照の対策
	stackId_.clear();
	
	for (const auto& object : attributes_) {
		OutlinerAttribute(object);
	}

	ImGui::End();

	// attribute
	ImGui::Begin("Attribute", &isOpenWindow, windowFlags);
	if (selectedAttribute_ != nullptr) {

		ImGui::SeparatorText(selectedAttribute_->GetName().c_str());

		//ImGui::InputText("name", name.data(), 20);
		//// HACK: 文字数0にすると止まる
		//// std::stringのsize以上に入力するとsize以上が表示されない

		//if (ImGui::Button("set")) {
		//	if (name.empty()) { //!< nameが空だった場合
		//		name = selectedAttribute_->GetName(); //!< 元に戻す
		//		SetLog("name can't set empty", errorColor);

		//	} else {
		//		selectedAttribute_->SetName(name.data());
		//	}
		//}

		selectedAttribute_->SetAttributeImGui();

	}

	ImGui::End();
}

void Console::OutputSystem() {
	static bool isOpenWindow = true;
	ImGui::Begin("System", &isOpenWindow, windowFlags);

	if (ImGui::CollapsingHeader("DescriptorHeaps")) {
		auto descriptorHeaps = dxCommon_->GetDescriptorsObj();
		
		if (ImGui::TreeNode("RTV")) {
			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(RTV),
				descriptorHeaps->GetIndexSize(RTV)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SRV & UAV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(SRV),
				descriptorHeaps->GetIndexSize(SRV)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("DSV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(DSV),
				descriptorHeaps->GetIndexSize(DSV)
			);

			ImGui::TreePop();
		}

	}

	ImGui::End();
}

void Console::SetTextureImGui(const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {
	{
		// タブ等を除いたウィンドウのサイズを取得(計算)
		ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
		ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

		// 元のアス比とImGuiウィンドウのアス比を比較
		float outerWindowAspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
		float innerWindowAspectRatio = wndSize.x / wndSize.y;
		ImVec2 finalWndSize = wndSize;
		
		if (outerWindowAspectRatio <= innerWindowAspectRatio) { // 横幅が大きかったら縦基準で画像サイズを決定
			finalWndSize.x *= outerWindowAspectRatio / innerWindowAspectRatio;

		} else { //縦幅が大きかったら横基準で画像サイズを決定
			finalWndSize.y *= innerWindowAspectRatio / outerWindowAspectRatio;
		}

		// 画像を中央に持ってくる
		ImVec2 topLeft = { 
			(wndSize.x - finalWndSize.x) * 0.5f + cntRegionMin.x,
			(wndSize.y - finalWndSize.y) * 0.5f + cntRegionMin.y
		};
		ImGui::SetCursorPos(topLeft);

		ImGui::Image((ImTextureID)texture.ptr, finalWndSize);
	}
}

void Console::OutlinerAttribute(Attribute* attribute) {

	// attributeが選択されてるか
	bool isSelect = (attribute == selectedAttribute_);

	// selectableに表示される名前
	std::string label = attribute->GetName();

	// 重複対策, ptrの値をstackIdとする
	label += "##" + std::format("{:p}", reinterpret_cast<void*>(attribute));

	if (attribute->GetNode().empty()) { //!< node構築がされてない場合
		if (ImGui::Selectable(label.c_str(), isSelect)) { //!< selectされた場合
			selectedAttribute_ = attribute;
		}

	} else {

		ImGuiTreeNodeFlags flags
			= ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_OpenOnArrow;

		if (isSelect) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(label.c_str(), flags)) {
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { //!< selectされた場合
				selectedAttribute_ = attribute;
			}

			for (const auto& node : attribute->GetNode()) {
				OutlinerAttribute(node);
			}

			ImGui::TreePop();
		}
	}
}

