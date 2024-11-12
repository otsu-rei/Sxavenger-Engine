#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Behavior/ModelBehavior.h>
#include <Engine/Game/Collider/Collider.h>

//* engine
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum EnemyType {
	kEnemyType_Star,
	kEnemyType_Cube,

	kCountOfEnemyType
};

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

	void Init(Model* model, const Vector3f& position, const Vector3f& velocity = {}, int32_t point = 30);

	void Term();

	void Update();

	void SetAttributeImGui() override;

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

	//* getter *//

	bool IsDelete() const { return isDelete_; }

	int32_t GetPoint() const { return point_; }

	Model* GetModel() const { return model_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* element parameter *//

	bool isDelete_ = false;

	Vector3f velocity_;
	// HACK: 敵1体にrailがありそれを使って動かす

	int32_t point_;
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

	////////////////////////////////////////////////////////////////////////////////////////////
	// EnemyPopCommand structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct EnemyPopCommand {
		DeltaTimePoint popTime;
		EnemyType type;
		Vector3f velocity;
		Vector3f position;
	};
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	std::list<std::unique_ptr<Enemy>> enemies_;

	//* command *//

	std::list<EnemyPopCommand> commands_;
	DeltaTimePoint popTimer_;

	//* element parameter *//

	Model* enemyModel_[kCountOfEnemyType] = { nullptr };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateEnemy(EnemyType type, const Vector3f& position, const Vector3f& velocity = {});

	void CreateEnemyPopCommand(DeltaTimePoint popTime, EnemyType type, const Vector3f& position, const Vector3f& velocity = {});

};