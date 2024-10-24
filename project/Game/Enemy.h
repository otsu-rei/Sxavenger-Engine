#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Behavior/ModelBehavior.h>
#include <Engine/Game/Collider/Collider.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy
	: public ModelBehavior, public Collider {
public:

	//=========================================================================================
	// public methods 
	//=========================================================================================

	Enemy()  = default;
	~Enemy() { Term(); }

	void Init(Model* model, const Vector3f& position);

	void Term();

	void Update();

	void SetAttributeImGui() override;

	//* getter *//

	bool IsDelete() const { return isDelete_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* element parameter *//

	bool isDelete_ = false;

};

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyCollection
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods 
	//=========================================================================================

	EnemyCollection()  = default;
	~EnemyCollection() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	std::list<std::unique_ptr<Enemy>> enemies_;

	//* element parameter *//

	Model* enemyModel_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateEnemy(const Vector3f& position);

};