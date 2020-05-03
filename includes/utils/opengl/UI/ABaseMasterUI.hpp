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
		void				addChild(ABaseUI * child);
		void				removeChild(ABaseUI * child);
		void				setMasterOffset(glm::vec2 offset);
		void				addMasterOffset(glm::vec2 offset);

		/* getter */
		virtual glm::vec2	getMasterPos() const;
		virtual glm::vec2	getMasterRealPos() const;
		virtual glm::vec2	getMasterSize() const;
		virtual glm::vec2	getMasterOffset() const;

	protected:
		virtual void	_update() = 0;
		virtual void	_draw() = 0;
		void			_updateTotalMasterSize();
		ABaseMasterUI();

		glm::vec2				_masterPadding;
		glm::vec2				_masterOffset;
		glm::vec2				_masterMinPos;
		glm::vec2				_masterMaxPos;
		glm::vec2				_masterTotalSize;
		std::vector<ABaseUI *>	_childUI;
};
