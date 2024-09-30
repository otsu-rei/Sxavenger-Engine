#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "teapot.obj");

	SetToConsole("teapot");
}

void Teapot::SetAttributeImGui() {
	ImGui::Text("[SetAttributeImGui]:: Test");
}
