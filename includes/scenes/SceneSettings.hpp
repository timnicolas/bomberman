#ifndef SCENESETTINGS_HPP
# define SCENESETTINGS_HPP

# include <list>
# include "ASceneMenu.hpp"
# include "Inputs.hpp"

namespace SettingsType {
	/**
	 * @brief Type of settings (there are 3 categories)
	 */
	enum 		Enum {
		GRAPHICS = 0,
		AUDIO = 1,
		CONTROLS = 2,
	};

	const int	nb_types = 3;  /**< number of settings categories */
}  // namespace SettingsType

/**
 * @brief Scene for settings
 */
class SceneSettings: public ASceneMenu {
public:
	/**
	 * @brief Contain width & height elements
	 */
	struct				res {
		int				width;  /**< Width */
		int				height;  /**< Height */
	};

	static const int			nb_resolution = 7;  /**< Number of default resolutions */
	static SceneSettings::res	resolutions[SceneSettings::nb_resolution];  /**< Description of all resolutions */
	static const std::string	audio_name[3];  /**< All audio names (master, music, sound) */

	bool					startFitToScreen;  /**< Launch program with 'fit to screen' option */

	virtual ~SceneSettings();
	explicit SceneSettings(Gui * gui, float const &dtTime);
	SceneSettings(SceneSettings const &src);

	SceneSettings				&operator=(SceneSettings const &rhs);

	virtual bool				init();
	virtual bool				update();

	void						updateAudioSliders();

	glm::ivec2					getCurResolution() const;

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

	float						_text_scale = 1.5f;  /**< Text scale */
	int							_input_configuring;  /**< Current input configuring */
	SceneSettings::res			_current_resolution;  /**< Current resolution */
	int							_select_res;  /**< Selected res */

	/* UI object */
	SettingsType::Enum			_current_pane;  /**< Current settings category displayed */
	std::list<ABaseUI*>			_panes[SettingsType::nb_types];  /**< All categories */
	ButtonUI					*_key_buttons[Inputs::nb_input];  /**< All inputs buttons to setting keys */
	ButtonUI					*_paneSelection[SettingsType::nb_types];  /**< All buttons at the top to choose category */
	ButtonUI					*_fullscreen_button;  /**< Button to set fullscreen option */
	ButtonUI					*_fit_to_screen_button;  /**< Button to set fit to screen option */
	TextUI						*_resolution_text;  /**< Resolution text (show current resolution) */
	TextUI						*_reloadWinText;  /**< Text to ask restarting program (if resolution has changed) */

	/* temporary settings */
	bool						_fullscreen;  /**< True if we are in fullscreen */
	float						_audio_volume[3];  /**< All audio volumes */
	SliderUI*					_volSliders[3];  /**< All audio sliders */

	/* UI listeners */
	bool						_return;  /**< Listener for return */
	bool						_reset;  /**< Listener for reset */
	bool						_next_resolution;  /**< Listener for next_resolution */
	bool						_prev_resolution;  /**< Listener for prev_resolution */
	bool						_update_fullscreen;  /**< Listener for update_fullscreen */
	bool						_update_fit_to_screen;  /**< Listener for update_fit_to_screen */
	bool						_save_audio[3];  /**< Listener for save_audio */
	float						_update_audio[3];  /**< Listener for update_audio */
	bool						_save_mouse_sens;  /**< Listener for save_mouse_sens */
	float						_update_mouse_sens;  /**< Listener for update_mouse_sens */
	bool						_select_pane[SettingsType::nb_types];  /**< Listener for select_pane */
	bool						_update_key[Inputs::nb_input];  /**< Listener for update_key */
};

#endif
