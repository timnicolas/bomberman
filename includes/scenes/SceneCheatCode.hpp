#pragma once

#include <deque>
#include "ASceneMenu.hpp"
#include "TextInputUI.hpp"

#define CHEATCODE_FONT				"cheatcode"
#define CHEATCODE_FONT_SCALE		1
#define CHEATCODE_COLOR				glm::vec4(0, 0, 0, 0.5)
#define CHEATCODE_TEXT_COlOR		glm::vec4(1, 1, 1, 1)
#define CHEATCODE_TEXT_ERR_COlOR	glm::vec4(1, 0, 0, 1)
#define CHEATCODE_DEF_TXT			"/"
#define CHEATCODE_TAB				"    "

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
		bool				evalCommand(std::string const & command);
		void				clearAllLn();

		bool				isCmdLnEnabled;

	private:
		SceneCheatCode();
		/* for parser */
		std::vector<std::string>	_splitCommand(std::string const & command) const;
		bool						_isSpace(char c) const;
		bool						_isValidCommand(std::string const & name) const;

		/* commands definition */
		typedef bool (SceneCheatCode::*execFnPtr)(std::vector<std::string> const &);
		struct Command {
			std::string	prototype;
			std::string	description;
			execFnPtr	exec;
		};

		/* commands functions */
		// bool _exec<cmd name>(std::vector<std::string> const & args);
		bool	_execHelp(std::vector<std::string> const & args);
		bool	_execClear(std::vector<std::string> const & args);

		/* for lines */
		void				_addLine(std::string const & txt, glm::vec4 txtColor = CHEATCODE_TEXT_COlOR);
		void				_removeLastLine();
		struct TextLine {
			ABaseUI *	ui;
		};

		/* variables */
		TextInputUI *			_commandLine;
		std::deque<TextLine>	_textLines;

		std::map<std::string, Command>	_commandsList;
};
