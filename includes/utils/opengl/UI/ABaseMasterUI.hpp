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
		/**
		 * @brief Update object. Called on every frames
		 *
		 * @return false If failed
		 */
		virtual void	_update() = 0;
		/**
		 * @brief Draw object. Called on every frames
		 *
		 * @return false If failed
		 */
		virtual void	_draw() = 0;
		void			_updateTotalMasterSize();
		ABaseMasterUI();

		glm::vec2				_masterPadding;  /**< Master padding */
		glm::vec2				_masterOffset;  /**< Master offset (used to move childs objects) */
		glm::vec2				_masterMinPos;  /**< Top position */
		glm::vec2				_masterMaxPos;  /**< Bottom position */
		glm::vec2				_masterTotalSize;  /**< Total size (size & pos of all childs) */
		std::vector<ABaseUI *>	_childUI;  /**< All childs reference */
};
