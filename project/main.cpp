//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Scene/GameScene.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	SxavengerSystem::Init();

	std::unique_ptr<GameScene> scene = std::make_unique<GameScene>();
	scene->Run();
	
	SxavengerSystem::Term();
	return 0;
}