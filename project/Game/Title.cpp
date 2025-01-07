#include "Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Title::Init() {
	texture_ = SxavengerAsset::TryImport<Texture>("asset/textures/title.png").Lock();
	SxavengerAsset::PushTask(texture_);
	texture_->WaitComplete();

	BaseBehavior::SetToConsole("aaa");
	BaseBehavior::SetRenderingFlag(BehaviorRenderingType::kLateAdaptive);
}

void Title::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	SxavengerModule::GetSpriteCommon()->DrawSprite(
		{}, kMainWindowSize, texture_->GetGPUHandleSRV()
	);
}
