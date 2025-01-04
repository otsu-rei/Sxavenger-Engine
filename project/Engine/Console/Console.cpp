#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::string Console::kConsoleName_ = "Sxavenger Engine Console";

////////////////////////////////////////////////////////////////////////////////////////////
// Console class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Console::Init() {
	runtime_.Begin();

	//* windowFlag
	windowFlag_ = 0;
	windowFlag_ |= ImGuiWindowFlags_NoCollapse;
	windowFlag_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlag_ |= ImGuiWindowFlags_NoMove;
	windowFlag_ |= ImGuiWindowFlags_NoResize;

	SystemConsole::Init(this);
	RenderConsole::Init(this);
	AssetConsole::Init(this);
}

void Console::Term() {
	SystemConsole::Term();
	RenderConsole::Term();
	AssetConsole::Term();

	runtime_.End();
	EngineLog(std::format("[Console] executed time: {:.0f}sec", runtime_.GetDeltaTime<TimeUnit::second>().time));
}

void Console::UpdateConsole() {
#ifdef _DEBUG
	BeginDisabled(!IsForcusConsoleWindow());

	DisplayMainMenu();

	if (isDisplayConsole_) {

		//!< docking用Idを取得
		dockingId_ = ImGui::GetID(kConsoleName_.c_str());

		DisplayPerformace();

		SystemConsole::UpdateConsole();
		RenderConsole::UpdateConsole();
		AssetConsole::UpdateConsole();
	}

	EndDisabled();
#endif // _DEBUG
}

void Console::Draw() {
	RenderConsole::Draw();
}

void Console::DrawConsole() {
	if (!subWindow_.expired()) {
		auto window = subWindow_.lock();

		window->BeginRendering();
		window->TryClearWindow();

		RenderConsole::PresentToScreen(window.get(), SxavengerSystem::GetMainThreadContext());

		window->EndRendering();
	}

	if (!window_.expired()) {
		auto window = window_.lock();

		window->BeginRendering();
		window->TryClearWindow();

		RenderConsole::PresentToScreen(window.get(), SxavengerSystem::GetMainThreadContext());
		
		SxavengerSystem::RenderImGui();
		window->EndRendering();
	}
}

void Console::DockingConsole() const {
	ImGui::SetNextWindowDockID(dockingId_, ImGuiCond_FirstUseEver);
}

void Console::SetWindow(const std::weak_ptr<GameWindow>& window) {
	window_ = window;

	RenderConsole::CreateFrame(window.lock()->GetSize());
}

Console* Console::GetInstance() {
	static Console instance;
	return &instance;
}

void Console::DisplayMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Config")) {
		MenuDummy();

		if (ImGui::BeginMenu("Console")) {
			MenuDummy();

			//* console config
			ImGui::SeparatorText("console");
			ImGui::Checkbox("display console", &isDisplayConsole_);

			// windowflag
			ImGuiWindowFlags lockWindow = 0;
			lockWindow |= ImGuiWindowFlags_NoMove;
			lockWindow |= ImGuiWindowFlags_NoResize;
			ImGui::CheckboxFlags("lock console", &windowFlag_, lockWindow);

			// .ini layout
			if (ImGui::Button("output layout ini")) {
				SxavengerSystem::GetImGuiController()->OutputLayout();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Layout")) {
			ImGui::Dummy({ 200.0f, 0.0f });
			//!< ImGui公式のstyleに関するeditor
			ImGui::ShowStyleEditor();
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window")) {
		MenuDummy();
		
		if (ImGui::BeginMenu("console window")) {
			MenuDummy();
			ImGui::Checkbox("display system console", &(SystemConsole::isDisplaySystemConsole_));
			ImGui::Checkbox("display render console", &(RenderConsole::isDisplayRenderConsole_));
			ImGui::Checkbox("display asset console",  &(AssetConsole::isDisplayAssetConsole_));
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("sub window")) {
			MenuDummy();

			if (subWindow_.expired()) {
				if (ImGui::Button("create sub window")) {
					if (!window_.expired()) {
						subWindow_ = SxavengerSystem::TryCreateSubWindow(window_.lock()->GetSize(), L"Sxavenger Console Window");
						subWindow_.lock()->SetWindowIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });
					}
				}


			} else {
				if (ImGui::Button("close window")) {
					auto window = subWindow_.lock();
					window->Close();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("System")) {
		MenuDummy();
		SystemConsole::ShowSystemMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Asset")) {
		MenuDummy();
		AssetConsole::ShowAssetMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Render")) {
		MenuDummy();
		RenderConsole::ShowRenderConsoleMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Debug")) {
		MenuDummy();
		RenderConsole::ShowDebugMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Graphics")) {
		MenuDummy();
		RenderConsole::ShowGraphicsMenu();
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void Console::DisplayPerformace() {
	if (SystemConsole::IsUpdateRequired()) {
		// windowの色を変更
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGuiController::ToImVec4({ 45, 5, 8, 255 }));
	}

	DockingConsole();
	ImGui::Begin("Performance ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	TimePointf<TimeUnit::second> framesPerSec = SxavengerSystem::GetPerformance()->GetDeltaTime();

	std::string text = "";
	text += std::format("[exec speed / frame]: {:.6f}", framesPerSec.time) + " ";
	text += std::format("[frame per second]: {:.1f}", 1.0f / framesPerSec.time);
	ImGui::Text(text.c_str());

	if (ImGui::BeginItemTooltip()) {
		std::vector<float> values(fpsHistory_.begin(), fpsHistory_.end());
		auto max = std::ranges::max_element(values);

		std::string overlay = std::format("max: {:.1f}", *max);

		ImGui::PlotLines("fps", values.data(), static_cast<int32_t>(values.size()), 0, overlay.c_str(), 0.0f, *max, {200.0f, 40.0f});
		ImGui::EndTooltip();
	}

	ImGui::End();

	if (SystemConsole::IsUpdateRequired()) {
		// historyに保存
		fpsHistory_.emplace_back(1.0f / framesPerSec.time);

		while (fpsHistory_.size() >= kHistoryCount_) {
			fpsHistory_.pop_front();
		}

		ImGui::PopStyleColor();
	}
}

bool Console::IsForcusConsoleWindow() {
	if (!window_.expired()) {
		auto window = window_.lock();
		return window.get() == SxavengerSystem::GetForcusWindow();
	}

	return false;
}

void Console::BeginDisabled(bool isDisable) {
	// 無効化状態のアルファ値を調整
	float alpha = 1.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, alpha);
	ImGui::BeginDisabled(isDisable);
}

void Console::EndDisabled() {
	ImGui::EndDisabled();
	ImGui::PopStyleVar(); // スタイルを元に戻す
}

void Console::MenuDummy() {
	static const ImVec2 size = { 200.0f, 0.0f };
	ImGui::Dummy(size);
}
