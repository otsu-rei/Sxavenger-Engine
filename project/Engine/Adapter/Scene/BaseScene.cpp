#include "BaseScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "SceneController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Base Scene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::RequestNextScene(const std::string& key) {
	controller_->RequestNextScene(key);
}
