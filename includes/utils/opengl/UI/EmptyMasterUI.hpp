#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"
#include "ABaseMasterUI.hpp"

/**
 * @brief this is the UI for invisible master object
 */
class EmptyMasterUI : public ABaseMasterUI {
	public:
		EmptyMasterUI(glm::vec2 pos, glm::vec2 size);
		EmptyMasterUI(EmptyMasterUI const & src);
		virtual ~EmptyMasterUI();

		EmptyMasterUI &		operator=(EmptyMasterUI const & rhs);
	protected:
		virtual void	_update();
		virtual void	_draw();
		EmptyMasterUI();
};
