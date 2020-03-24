#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for all master objects
 *
 * Master objects are objects that contains others objects (for example the scrollbar is a master object)
 */
class ABaseMasterUI : public ABaseUI {
	public:
		ABaseMasterUI(glm::vec2 pos, glm::vec2 size);
		ABaseMasterUI(ABaseMasterUI const & src);
		virtual ~ABaseMasterUI();

		ABaseMasterUI & operator=(ABaseMasterUI const & rhs);

		/* setter */
		void			addChild(ABaseUI * child);
		void			removeChild(ABaseUI * child);

		/* getter */
		glm::vec2		getMasterPos() const;
		glm::vec2		getMasterSize() const;
		glm::vec2		getTotalMasterSize() const;

	protected:
		virtual void	_update() = 0;
		virtual void	_draw() = 0;
		ABaseMasterUI();

		std::vector<ABaseUI *>	_childUI;
};
