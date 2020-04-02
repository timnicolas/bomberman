#pragma once

#include <deque>
#include "ASceneMenu.hpp"
#include "TextInputUI.hpp"

#define CHEATCODE_FONT			"cheatcode"
#define CHEATCODE_FONT_SCALE	1
#define CHEATCODE_COLOR			glm::vec4(0, 0, 0, 0.5)
#define CHEATCODE_TEXT_COlOR	glm::vec4(1, 1, 1, 1)
#define CHEATCODE_DEF_TXT		"/"

/**
 * @brief this is the cheat code command line
 */
class SceneCheatCode : public ASceneMenu {
	public:
		// Constructors
		SceneCheatCode(Gui * gui, float const &dtTime);
		virtual ~SceneCheatCode();
		SceneCheatCode(SceneCheatCode const &src);

		// Operators
		SceneCheatCode &operator=(SceneCheatCode const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneCheatCode& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();
		virtual void		unload();

	private:
		SceneCheatCode();
		void				_addLine(std::string const & txt, glm::vec4 txtColor = CHEATCODE_TEXT_COlOR);
		void				_removeLastLine();

		struct TextLine {
			ABaseUI *	ui;
		};

		TextInputUI *			_commandLine;
		std::deque<TextLine>	_textLines;
};
