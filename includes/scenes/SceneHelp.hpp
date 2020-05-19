#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
class SceneHelp : public ASceneMenu {
	public:
		// Constructors
		SceneHelp(Gui * gui, float const &dtTime);
		virtual ~SceneHelp();
		SceneHelp(SceneHelp const &src);

		// Operators
		SceneHelp &operator=(SceneHelp const &rhs);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();
		void				setPage(int32_t page, bool enableTransition = true);

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	exit;  /**< True if we clicked on the exit button */
			bool	leftMenu;  /**< True if we clicked on the left menu button */
			bool	rightMenu;  /**< True if we clicked on the right menu button */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI	*								detonatorText;  /**< UI for text of detonator */
			ABaseUI	*								leftPage;  /**< UI for left page button */
			ABaseUI	*								rightPage;  /**< UI for right page button */
			std::vector< std::vector< ABaseUI* > >	allPages;  /**< UI for all pages */
		};
		AllUI		allUI;  /**< All UI elements */
		std::string	_lastSceneName;  /**< The last scene name */
		int32_t		_currentPage;  /**< Current page ID */
		uint32_t	_nbPages;  /**< Total nomber of pages */

		float		_transition;  /**< Used to do a smooth transition */

	private:
		SceneHelp();
};
