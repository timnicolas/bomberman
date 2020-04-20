#ifndef SCENESETTINGS_HPP
# define SCENESETTINGS_HPP

# include <list>
# include "ASceneMenu.hpp"
# include "Inputs.hpp"

namespace SettingsType {
	enum 		Enum {
		GRAPHICS = 0,
		AUDIO = 1,
		CONTROLS = 2,
	};

	const int	nb_types = 3;
}

class SceneSettings: public ASceneMenu {
public:
	struct				res {
		int				width;
		int				height;
	};

	static const int			nb_resolution = 7;
	static SceneSettings::res	resolutions[SceneSettings::nb_resolution];
	static const std::string	audio_name[3];

	virtual ~SceneSettings();
	explicit SceneSettings(Gui * gui, float const &dtTime);
	SceneSettings(SceneSettings const &src);

	SceneSettings				&operator=(SceneSettings const &rhs);

	virtual bool				init();
	virtual bool				update();

private:
	SceneSettings();

	void						_saveAudioVolume(int audio_index);
	void						_updateAudioVolume(int audio_index);
	void						_updateKey(InputType::Enum key_type);
	void						_selectPane(SettingsType::Enum pane_type);
	void						_updateMouseSensitivity();
	void						_updateFullscreen();
	void						_updateResolution(bool go_right);
	void						_resetKeys();
	void						_returnQuit();
	void						_cancelQuit();

	void						_init_graphics_pane(glm::vec2 tmp_pos, float menu_width,
		float menu_height);
	void						_init_audio_pane(glm::vec2 tmp_pos, float menu_width,
		float menu_height);
	void						_init_control_pane(glm::vec2 tmp_pos, float menu_width,
		float menu_height);

	void						_updateFullscreenButton();
	void						_updateResolutionText();

	float						_text_scale = 1.5f;
	int							_input_configuring;
	SceneSettings::res			_current_resolution;
	int							_select_res;

	/* UI object */
	SettingsType::Enum			_current_pane;
	std::list<ABaseUI*>			_panes[SettingsType::nb_types];
	ButtonUI					*_key_buttons[Inputs::nb_input];
	ButtonUI					*_paneSelection[SettingsType::nb_types];
	ButtonUI					*_fullscreen_button;
	TextUI						*_resolution_text;
	TextUI						*_reloadWinText;

	/* temporary settings */
	bool						_fullscreen;
	float						_audio_volume[3];

	/* UI listeners */
	bool						_return;
	bool						_reset;
	bool						_next_resolution;
	bool						_prev_resolution;
	bool						_update_fullscreen;
	bool						_save_audio[3];
	float						_update_audio[3];
	bool						_save_mouse_sens;
	float						_update_mouse_sens;
	bool						_select_pane[SettingsType::nb_types];
	bool						_update_key[Inputs::nb_input];
};

#endif
