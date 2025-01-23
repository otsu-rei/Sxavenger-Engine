#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

#include "Engine/!Render/FRenderCore.h"
#include "Engine/System/Config/SxavengerDirectory.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->TryGetEditor<EngineDeveloperEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"beta system window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	texture_ = SxavengerAsset::TryImport<AssetTexture>("asset/textures/uvChecker.png").WaitGet();

	index_ = std::make_unique<DxObject::DimensionBuffer<uint32_t>>();
	index_->Create(SxavengerSystem::GetDxDevice(), 1);
	index_->At(0) = texture_->GetDescriptor().GetIndex();

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();

	triangle_.Create(4, 2);
	triangle_.GetVertex()->At(0) = { { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f } };
	triangle_.GetVertex()->At(1) = { {  0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f } };
	triangle_.GetVertex()->At(2) = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f } };
	triangle_.GetVertex()->At(3) = { {  0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f } };

	triangle_.GetIndex()->At(0) = { 0, 2, 1 };
	triangle_.GetIndex()->At(1) = { 2, 3, 1 };

	pipeline_ = std::make_unique<DxObject::GraphicsPipelineState>();
	pipeline_->CreateBlob("asset/shaders/sample.vs.hlsl", DxObject::GraphicsShaderType::vs);
	pipeline_->CreateBlob("asset/shaders/sample.ps.hlsl", DxObject::GraphicsShaderType::ps);

	DxObject::GraphicsRootSignatureDesc root = {};
	root.SetVirtualCBV(0, DxObject::ShaderVisibility::VISIBILITY_PIXEL, 0);
	root.SetVirtualCBV(1, DxObject::ShaderVisibility::VISIBILITY_VERTEX, 1);
	root.SetSamplerLinear(DxObject::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(SxavengerSystem::GetDxDevice(), std::move(root));

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.ClearElement();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	desc.SetRTVFormat(0, DxObject::kScreenFormat);
	desc.SetDepthStencil(false);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
}

void BetaSystemGameLoop::DrawSystem() {

	main_->BeginRendering();
	main_->ClearWindow();

	auto commandList = SxavengerSystem::GetMainThreadContext()->GetCommandList();

	pipeline_->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), main_->GetSize());

	triangle_.BindIABuffer(SxavengerSystem::GetMainThreadContext());

	commandList->SetGraphicsRootConstantBufferView(0, index_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, camera_->GetGPUVirtualAddress());

	triangle_.DrawCall(SxavengerSystem::GetMainThreadContext());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
