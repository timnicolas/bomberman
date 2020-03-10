#ifndef SCENESETTINGS_HPP
# define SCENESETTINGS_HPP

# include <list>
# include "SceneMenu.hpp"
# include "Inputs.hpp"

namespace SettingsType {
	enum 		Enum {
		GRAPHICS = 0,
		AUDIO = 1,
		CONTROLS = 2,
	};

	const int	nb_types = 3;
}

class SceneSettings: public SceneMenu {
public:
	virtual ~SceneSettings();
	explicit SceneSettings(Gui * gui, float const &dtTime);
	SceneSettings(SceneSettings const &src);

	SceneSettings		&operator=(SceneSettings const &rhs);

	virtual bool		init();
	virtual bool		update();

private:
	SceneSettings();

	void				_updateAudioVolume(std::string volume_name);
	void				_updateKey(InputType::Enum key_type);
	void				_selectPane(SettingsType::Enum pane_type);
	void				_updateFullscreen();
	void				_confirmQuit();
	void				_cancelQuit();

	void				_init_control_pane(glm::vec2 tmp_pos, float menu_width, float menu_height);

	/* UI object */
	SettingsType::Enum	_current_pane;
	std::list<ABaseUI*>	_panes[SettingsType::nb_types];

	/* temporary settings */
	bool				_fullscreen;

	/* UI listeners */
	bool				_confirm;
	bool				_cancel;
	bool				_next_resolution;
	bool				_prev_resolution;
	bool				_update_fullscreen;
	bool				_update_audio[3];
	bool				_select_pane[SettingsType::nb_types];
	bool				_update_key[Inputs::nb_input];
};

#endif
