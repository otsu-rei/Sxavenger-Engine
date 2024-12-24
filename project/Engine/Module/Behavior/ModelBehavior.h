#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "TransformBehavior.h"

//* engine
#include <Engine/Asset/Model/Model.h>
#include <Engine/Module/Material/MaterialComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelBehavior
	: public TransformBehavior, public MaterialComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelBehavior()  { Init(); }
	~ModelBehavior() = default;

	void Init();

	//* derivative behaivor methods *//
	//* ImGui command

	virtual void SystemAttributeImGui() override;

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Model* model_; //!< CONSIDER: std::shared_ptr<Model> model_;

};
