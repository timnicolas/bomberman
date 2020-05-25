#include "SceneSettings.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"
#include "Gui.hpp"
#include "Bonus.hpp"

/**
 * @brief Destroy the Scene Settings:: Scene Settings object
 */
SceneSettings::~SceneSettings() {}

/**
 * @brief Construct a new Scene Settings:: Scene Settings object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneSettings::SceneSettings(Gui *gui, float const &dtTime) : ASceneMenu(gui, dtTime),
	_input_configuring(-1),
	_current_pane(SettingsType::GRAPHICS),
	_return(false),
	_reset(false),
	_next_resolution(false),
	_prev_resolution(false),
	_update_fullscreen(false)
{
	for (auto i = 0; i < 3; i++) {
		_save_audio[i] = false;
		_update_audio[i] = 100 * s.j("audio").d(audio_name[i]);
		_audio_volume[i] = _update_audio[i];
	}
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		_select_pane[i] = false;
		_panes[i] = std::list<ABaseUI*>();
	}
	for (auto i = 0; i < Inputs::nb_input; i++) {
		_update_key[i] = false;
		_key_buttons[i] = nullptr;
	}
	_save_mouse_sens = false;
	_update_mouse_sens = s.d("mouse_sensitivity");
	_fullscreen_button = nullptr;
	_fullscreen = s.j("graphics").b("fullscreen");
	_select_res = -1;  // setted in init function
	_text_scale = static_cast<float>(s.j("graphics").i("width")) * 0.001;
}

/**
 * @brief Construct a new Scene Settings:: Scene Settings object
 *
 * @param src The object to do the copy
 */
SceneSettings::SceneSettings(SceneSettings const &src) : ASceneMenu(src) {
	*this = src;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneSettings& A reference to the copied object
 */
SceneSettings			&SceneSettings::operator=(SceneSettings const &rhs) {
	ASceneMenu::operator=(rhs);
	_input_configuring = rhs._input_configuring;
	_fullscreen = rhs._fullscreen;
	_return = rhs._return;
	_reset = rhs._reset;
	_current_pane = rhs._current_pane;
	_next_resolution = rhs._next_resolution;
	_prev_resolution = rhs._prev_resolution;
	_update_fullscreen = rhs._update_fullscreen;
	for (auto i = 0; i < 3; i++) {
		_save_audio[i] = rhs._save_audio[i];
		_update_audio[i] = rhs._update_audio[i];
		_audio_volume[i] = rhs._audio_volume[i];
	}
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		_select_pane[i] = rhs._select_pane[i];
		_panes[i] = rhs._panes[i];
	}
	for (auto i = 0; i < Inputs::nb_input; i++) {
		_update_key[i] = rhs._update_key[i];
		_key_buttons[i] = rhs._key_buttons[i];
	}
	_save_mouse_sens = rhs._save_mouse_sens;
	_update_mouse_sens = rhs._update_mouse_sens;
	_fullscreen_button = rhs._fullscreen_button;
	_text_scale = rhs._text_scale;
	_select_res = rhs._select_res;
	return *this;
}

SceneSettings::res		SceneSettings::resolutions[SceneSettings::nb_resolution] = {
	{800, 600},
	{1080, 720},
	{1200, 800},
	{1600, 900},
	{1920, 1080},
	{2560, 1440}
};

const std::string		SceneSettings::audio_name[3] = {
	"master",
	"music",
	"sound",
};

/**
 * @brief initialize all the UI section. Should not be called twice.
 */
bool					SceneSettings::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x * 0.8;
	float menuHeight = winSz.y / 14;

	startFitToScreen = s.j("graphics").b("fitToScreen");
	if (startFitToScreen) {
		_select_res = 0;
	}
	else {
		_select_res = -1;
		for (int i = 0; i < SceneSettings::nb_resolution; i++) {
			if (resolutions[i].width == s.j("graphics").i("width")
			&& resolutions[i].height == s.j("graphics").i("height"))
			{
				_select_res = i;
				break;
			}
		}
		if (_select_res == -1) {
			logErr("Invalid resolution " << s.j("graphics").i("width") << "x" << s.j("graphics").i("height"));
			s.j("graphics").i("width") = resolutions[0].width;
			s.j("graphics").i("height") = resolutions[0].height;
			_gui->gameInfo.savedWindowSize.x = s.j("graphics").i("width");
			_gui->gameInfo.savedWindowSize.y = s.j("graphics").i("height");
			logInfo("save new resolution " << s.j("graphics").i("width") << "x" << s.j("graphics").i("height"));
			saveSettings(homeDir+SETTINGS_FILE);
			return false;
		}
	}
	_current_resolution = SceneSettings::resolutions[_select_res];

	try {
		addExitButton()
			.addButtonLeftListener(&_return);
		// title
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Settings");

		// buttons
		tmpSize.y = menuHeight;
		tmpSize.x = menuWidth / 3 * 0.9;
		tmpPos.y -= menuHeight * 1.8;
		tmpPos.x += (menuWidth / 3 - tmpSize.x) / 2;
		_paneSelection[SettingsType::GRAPHICS] = reinterpret_cast<ButtonUI*>(&addButton(tmpPos, tmpSize, "Graphics")
			.setSelected(true)
			.addButtonLeftListener(&_select_pane[SettingsType::GRAPHICS])
			.setTextScale(_text_scale)
			.setTextAlign(TextAlign::CENTER));
		tmpPos.x += (menuWidth / 3);
		_paneSelection[SettingsType::AUDIO] = reinterpret_cast<ButtonUI*>(&addButton(tmpPos, tmpSize, "Audio")
			.addButtonLeftListener(&_select_pane[SettingsType::AUDIO])
			.setKeyLeftClickScancode(SDL_SCANCODE_2)
			.setTextScale(_text_scale)
			.setTextAlign(TextAlign::CENTER));
		tmpPos.x += (menuWidth / 3);
		_paneSelection[SettingsType::CONTROLS] = reinterpret_cast<ButtonUI*>(&addButton(tmpPos, tmpSize, "Controls")
			.addButtonLeftListener(&_select_pane[SettingsType::CONTROLS])
			.setKeyLeftClickScancode(SDL_SCANCODE_3)
			.setTextScale(_text_scale)
			.setTextAlign(TextAlign::CENTER));

		tmpPos.y -= menuHeight * 1.8;

		/* graphics */
		_init_graphics_pane(tmpPos, menuWidth, menuHeight);
		/* audio */
		_init_audio_pane(tmpPos, menuWidth, menuHeight);
		/* controls */
		_init_control_pane(tmpPos, menuWidth, menuHeight);
		AudioManager::loadSound("sounds/bell.ogg");

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

/**
 * @brief initialize the graphics section. Should not be called twice.
 */
void					SceneSettings::_init_graphics_pane(glm::vec2 tmpPos, float menuWidth, float menuHeight) {
	glm::vec2	winSz = _gui->gameInfo.windowSize;
	glm::vec2	tmpSize;
	ABaseUI		*ptr;

	/* fullscreen button */
	tmpSize.y = menuHeight;
	tmpSize.x = menuWidth / 3;
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	ptr = &addText(tmpPos, tmpSize, "Fullscreen :")
		.setTextAlign(TextAlign::RIGHT)
		.setTextScale(_text_scale)
		.setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpSize.x = menuWidth / 10;
	tmpPos.x += menuWidth / 2 + menuWidth / 6 - tmpSize.x / 2;
	_fullscreen_button = &addButton(tmpPos, tmpSize, "OFF");
	ptr = &_fullscreen_button->addButtonLeftListener(&_update_fullscreen)
		.setKeyLeftClickScancode(SDL_SCANCODE_F)
		.setTextScale(_text_scale)
		.setTextAlign(TextAlign::CENTER);
	_updateFullscreenButton();
	_panes[SettingsType::GRAPHICS].push_front(ptr);

	/* fit to screen */
	tmpSize.y = menuHeight;
	tmpSize.x = menuWidth / 3;
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	tmpPos.y -= tmpSize.y * 1.3;
	ptr = &addText(tmpPos, tmpSize, "Fit  to  screen :")
		.setTextAlign(TextAlign::RIGHT)
		.setTextScale(_text_scale)
		.setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpSize.x = menuWidth / 10;
	tmpPos.x += menuWidth / 2 + menuWidth / 6 - tmpSize.x / 2;
	_fit_to_screen_button = &addButton(tmpPos, tmpSize, "OFF");
	ptr = &_fit_to_screen_button->addButtonLeftListener(&_update_fit_to_screen)
		.setKeyLeftClickScancode(SDL_SCANCODE_O)
		.setTextScale(_text_scale)
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::GRAPHICS].push_front(ptr);

	/* resolution choice */
	tmpSize.x = menuWidth / 3;
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	tmpPos.y -= tmpSize.y * 1.3;
	ptr = &addText(tmpPos, tmpSize, "Resolution :")
		.setTextAlign(TextAlign::RIGHT)
		.setTextScale(_text_scale)
		.setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpPos.x += (menuWidth / 2);
	_resolution_text = &addText(tmpPos, tmpSize, "800x600");
	ptr = &_resolution_text->setTextAlign(TextAlign::CENTER)
		.setTextScale(_text_scale)
		.setEnabled(true);
	_updateResolutionText();
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpSize.x = menuWidth / 14;
	tmpPos.x -= tmpSize.x;
	ptr = &addButtonImage(tmpPos, {tmpSize.x, 0}, s.s("imgsUI") + "/prev.png", s.s("imgsUI") + "/prev_hover.png")
		.setKeyLeftClickScancode(SDL_SCANCODE_LEFT)
		.addButtonLeftListener(&_prev_resolution)
		.setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpPos.x += menuWidth / 3 + tmpSize.x;
	ptr = &addButtonImage(tmpPos, {tmpSize.x, 0}, s.s("imgsUI") + "/next.png", s.s("imgsUI") + "/next_hover.png")
		.setKeyLeftClickScancode(SDL_SCANCODE_RIGHT)
		.addButtonLeftListener(&_next_resolution)
		.setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmpPos.x = (menuWidth / 2);
	tmpPos.y -= ptr->getSize().y;
	_reloadWinText = &addText(tmpPos, tmpSize, "Restart  the  game  to  apply  new  resolution");
	ptr = &_reloadWinText->setTextAlign(TextAlign::CENTER)
		.setTextScale(_text_scale)
		.setTextColor(colorise(s.j("colors").j("red").u("color")))
		.setEnabled(false);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	// border rectangle
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	tmpSize.x = menuWidth;
	tmpSize.y = winSz.y - tmpPos.y - menuHeight * 2.3;
	ptr = &addRect(tmpPos, tmpSize).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
}

/**
 * @brief initialize the audio section. Should not be called twice.
 */
void					SceneSettings::_init_audio_pane(glm::vec2 tmpPos, float menuWidth, float menuHeight) {
	glm::vec2	winSz = _gui->gameInfo.windowSize;
	glm::vec2	tmpSize;
	ABaseUI		*ptr;
	float		tmpVal;

	tmpSize.y = menuHeight;
	for (auto i = 0; i < 3; i++) {
		tmpSize.x = menuWidth / 3;
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		if (i != 0)
			tmpPos.y -= menuHeight * 1.3;
		ptr = &addText(tmpPos, tmpSize, SceneSettings::audio_name[i] + " :").setTextAlign(TextAlign::RIGHT) \
			.setTextScale(_text_scale).setEnabled(false);
		_panes[SettingsType::AUDIO].push_front(ptr);
		tmpSize.x *= 2;
		tmpSize.x -= 10 * 2;
		tmpPos.x += (menuWidth / 3) + 10;
		tmpVal = s.j("audio").d(SceneSettings::audio_name[i]);
		ptr = &addSlider(tmpPos, tmpSize, 0, 100, tmpVal * 100, 1).addSliderListener(&_update_audio[i]) \
			.addButtonLeftListener(&_save_audio[i]).setTextScale(_text_scale).setEnabled(false);
		_volSliders[i] = reinterpret_cast<SliderUI*>(ptr);
		_panes[SettingsType::AUDIO].push_front(ptr);
	}
	// border rectangle
	tmpPos.y -= tmpSize.y * 0.5;
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	tmpSize.x = menuWidth;
	tmpSize.y = winSz.y - tmpPos.y - menuHeight * 2.3;
	ptr = &addRect(tmpPos, tmpSize).setEnabled(false);
	_panes[SettingsType::AUDIO].push_front(ptr);
}

/**
 * @brief initialize the controls section. Should not be called twice.
 */
void					SceneSettings::_init_control_pane(glm::vec2 tmpPos, float menuWidth, float menuHeight) {
	glm::vec2		winSz = _gui->gameInfo.windowSize;
	glm::vec2		tmpSize;
	ABaseUI *		ptr;
	std::string		keyName;
	ABaseMasterUI *	scrollbar;
	float			keyMenuWidth = menuWidth / 1.5;
	float			keyMenuHeight = winSz.x / 25;
	float			keyMenuPadding = winSz.x * 0.01;

	Bonus::description[BonusType::DETONATOR] = DETONATOR_DESC \
		" (" + Inputs::getKeyName(InputType::ACTION_2) + ")";

	// create scrollbar
	tmpSize.x = (keyMenuWidth + 3 * keyMenuPadding);
	tmpSize.y = menuHeight * 1.3 * 6;
	tmpPos.x = (winSz.x / 2) - ((keyMenuWidth + 2 * keyMenuPadding) / 2);
	tmpPos.y -= tmpSize.y - menuHeight * 1.3;
	ptr = &addScrollbar(tmpPos, tmpSize)
		.enableVertScroll(true)
		.setEnabled(false);
	scrollbar = reinterpret_cast<ABaseMasterUI*>(ptr);
	_panes[SettingsType::CONTROLS].push_front(ptr);

	// border rectangle position
	glm::vec2 border_pos = tmpPos;
	border_pos.y -= menuHeight / 2;
	border_pos.x = (winSz.x / 2) - (menuWidth / 2);

	// add buttons
	tmpPos.y = scrollbar->getMasterSize().y;
	tmpSize.x = (keyMenuWidth - keyMenuPadding) / 2;
	tmpSize.y = keyMenuHeight;
	for (auto i = 0; i < Inputs::nb_input; i++) {
		if (Inputs::input_type_name[i] == "cancel") {
			_key_buttons[i] = nullptr;
			continue;
		}
		tmpPos.y -= keyMenuHeight + keyMenuPadding;
		tmpPos.x = 0;
		ptr = &addText(tmpPos, tmpSize, Inputs::input_type_name[i] + " :")
			.setTextAlign(TextAlign::RIGHT)
			.setTextScale(_text_scale)
			.setMaster(scrollbar)
			.setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
		tmpPos.x = scrollbar->getMasterSize().x - (tmpSize.x + keyMenuPadding);
		keyName = Inputs::getKeyName(static_cast<InputType::Enum>(i));
		ptr = &addButton(tmpPos, tmpSize, keyName)
			.addButtonLeftListener(&_update_key[i])
			.setTextAlign(TextAlign::CENTER)
			.setTextScale(_text_scale)
			.setMaster(scrollbar)
			.setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
		_key_buttons[i] = reinterpret_cast<ButtonUI*>(ptr);
	}
	// add mouse sensitivity slider
	#if DEBUG
		tmpPos.y -= keyMenuHeight + keyMenuPadding;
		tmpPos.x = 0;
		ptr = &addText(tmpPos, tmpSize, "mouse sensitivity :")
			.setTextAlign(TextAlign::RIGHT)
			.setTextScale(_text_scale)
			.setMaster(scrollbar)
			.setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
		tmpPos.x = scrollbar->getMasterSize().x - (tmpSize.x + keyMenuPadding);
		ptr = &addSlider(tmpPos, tmpSize, 0.1, 3, _update_mouse_sens, 0.05)
			.addSliderListener(&_update_mouse_sens)
			.addButtonLeftListener(&_save_mouse_sens)
			.setTextScale(_text_scale)
			.setMaster(scrollbar)
			.setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
	#endif
	// reset
	tmpSize.y = menuHeight;
	tmpSize.x = menuWidth * 0.15;
	tmpPos.x = (winSz.x / 2) + (menuWidth / 2) - menuWidth * 0.1;
	tmpPos.y = winSz.y - menuHeight * 1.3;
	ptr = &addButton(tmpPos, tmpSize, "Reset")
		.addButtonLeftListener(&_reset)
		.setTextScale(_text_scale)
		.setTextAlign(TextAlign::CENTER)
		.setEnabled(false);
	_panes[SettingsType::CONTROLS].push_front(ptr);

	// border rectangle
	tmpSize.x = menuWidth;
	tmpSize.y = winSz.y - border_pos.y - menuHeight * 2.3;
	ptr = &addRect(border_pos, tmpSize).setEnabled(false);
	_panes[SettingsType::CONTROLS].push_front(ptr);
}

/**
 * @brief Change the display of the resolution button according to the resolution setting.
 */
void					SceneSettings::_updateResolutionText() {
	std::string		text = std::to_string(_current_resolution.width) + "x" \
		+ std::to_string(_current_resolution.height);
	_resolution_text->setText(text);
}

/**
 * @brief Change the display of the fullscreen button according to the fullscreen setting.
 */
void					SceneSettings::_updateFullscreenButton() {
	if (_fullscreen_button != nullptr) {
		std::string symbol;
		if (_fullscreen) {
			symbol = "ON";
		}
		else {
			symbol = "OFF";
		}
		_fullscreen_button->setText(symbol).setSelected(_fullscreen);
	}
}

/**
 * @brief React to the user input at each frame.
 */
bool					SceneSettings::update() {
	ASceneMenu::update();
	if (_reset) {
		_resetKeys();
	}
	if (_input_configuring >= 0) {
		_paneSelection[SettingsType::GRAPHICS]->setKeyLeftClickScancode(NO_SCANCODE);
		_paneSelection[SettingsType::AUDIO]->setKeyLeftClickScancode(NO_SCANCODE);
		_paneSelection[SettingsType::CONTROLS]->setKeyLeftClickScancode(NO_SCANCODE);
		if (!Inputs::isConfiguring()) {
			_key_buttons[_input_configuring]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(_input_configuring)));
			if (static_cast<InputType::Enum>(_input_configuring) == InputType::ACTION_2) {
				Bonus::description[BonusType::DETONATOR] = DETONATOR_DESC \
					" (" + Inputs::getKeyName(InputType::ACTION_2) + ")";
			}
			_input_configuring = -1;
		}
		else if (Inputs::getKeyByScancodeUp(DEFAULT_CANCEL)) {
			Inputs::cancelConfiguration();
			_key_buttons[_input_configuring]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(_input_configuring)));
			_input_configuring = -1;
		}
	}
	else {
		_paneSelection[SettingsType::GRAPHICS]->setKeyLeftClickScancode(SDL_SCANCODE_1);
		_paneSelection[SettingsType::AUDIO]->setKeyLeftClickScancode(SDL_SCANCODE_2);
		_paneSelection[SettingsType::CONTROLS]->setKeyLeftClickScancode(SDL_SCANCODE_3);
		for (auto i = 0; i < SettingsType::nb_types; i++) {
			if (_select_pane[i]) {
				_selectPane(static_cast<SettingsType::Enum>(i));
			}
		}
		for (auto i = 0; i < Inputs::nb_input; i++) {
			if (_update_key[i]) {
				_updateKey(static_cast<InputType::Enum>(i));
			}
		}
		for (auto i = 0; i < 3; i++) {
			if (_update_audio[i] != _audio_volume[i]) {
				_updateAudioVolume(i);
			}
			if (_save_audio[i]) {
				_saveAudioVolume(i);
			}
		}
		if (_save_mouse_sens) {
			_updateMouseSensitivity();
		}
		if (_update_fullscreen) {
			_updateFullscreen();
		}
		if (_update_fit_to_screen) {
			s.j("graphics").b("fitToScreen") = !s.j("graphics").b("fitToScreen");
			_update_fit_to_screen = false;
		}
		if (_prev_resolution) {
			_updateResolution(false);
		}
		if (_next_resolution) {
			_updateResolution(true);
		}
		if (_return) {
			_returnQuit();
		}
		if (_current_pane == SettingsType::GRAPHICS) {
			if (_gui->gameInfo.isSavedFullscreen != s.j("graphics").b("fullscreen")
			|| _gui->gameInfo.savedWindowSize.x != s.j("graphics").i("width")
			|| _gui->gameInfo.savedWindowSize.y != s.j("graphics").i("height")
			|| startFitToScreen != s.j("graphics").b("fitToScreen")) {
				_reloadWinText->setEnabled(true);
			}
			else {
				_reloadWinText->setEnabled(false);
			}
		}
		std::string symbol;
		if (s.j("graphics").b("fitToScreen")) {
			symbol = "ON";
		}
		else {
			symbol = "OFF";
		}
		_fit_to_screen_button->setText(symbol).setSelected(s.j("graphics").b("fitToScreen"));
		if (Inputs::getKeyDown(InputType::ACTION)) {
			try {
				AudioManager::playSound("sounds/bell.ogg");
			}
			catch (Sound::SoundException const & e) {
				logErr(e.what());
			}
		}
	}
	return true;
}

/**
 * @brief If audio level has changed (/volume master 12), update sliders
 */
void					SceneSettings::updateAudioSliders() {
	for (auto i = 0; i < 3; i++) {
		_volSliders[i]->setValue(s.j("audio").d(SceneSettings::audio_name[i]) * 100);
	}
}

/**
 * @brief disable and enable multiple UI element according to their section.
 *
 * @param pane_type the id of the section to enable. Other section will be disabled.
 */
void					SceneSettings::_selectPane(SettingsType::Enum pane_type) {
	_select_pane[pane_type] = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(_input_configuring)));
		_input_configuring = -1;
	}
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		for (auto it = _panes[i].begin(); it != _panes[i].end(); it++) {
			(*it)->setEnabled(i == pane_type);
			_paneSelection[i]->setSelected(i == pane_type);
		}
	}
}

/**
 * @brief enable the input configuration for the given input type.
 *
 * @param key_type the type of the input to configure.
 */
void					SceneSettings::_updateKey(InputType::Enum key_type) {
	_update_key[key_type] = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getKeyName(
			static_cast<InputType::Enum>(_input_configuring)));
	}
	logInfo("Start configuring key...");
	_key_buttons[key_type]->setText("...");
	_input_configuring = key_type;
	Inputs::configureKey(key_type);
}

/**
 * @brief update the audio volume for sound and/or music.
 *
 * @param audio_index the audio settings that has been modified.
 */
void					SceneSettings::_updateAudioVolume(int audio_index) {
	std::string volume_name =		 SceneSettings::audio_name[audio_index];
	float volume = _update_audio[audio_index];
	_audio_volume[audio_index] = _update_audio[audio_index];
	s.j("audio").d(volume_name) = volume / 100;
	AudioManager::updateSettings();
}

/**
 * @brief Save the audio volume in the settings file.
 *
 * @param audio_index the audio settings that has been modified.
 */
void					SceneSettings::_saveAudioVolume(int audio_index) {
	_save_audio[audio_index] = false;
	saveSettings(homeDir+SETTINGS_FILE);
}

/**
 * @brief Save the mouse sensitivity in the settings file.
 */
void					SceneSettings::_updateMouseSensitivity() {
	_save_mouse_sens = false;
	s.d("mouse_sensitivity") = _update_mouse_sens;
	saveSettings(homeDir+SETTINGS_FILE);
}

/**
 * @brief Enable or disable the fullscreen.
 */
void					SceneSettings::_updateFullscreen() {
	_update_fullscreen = false;
	_fullscreen = !_fullscreen;
	_updateFullscreenButton();
	#if RESTART_TO_UPDATE_RESOLUTION
		_gui->gameInfo.isSavedFullscreen = _fullscreen;
	#else
		s.j("graphics").b("fullscreen") = _fullscreen;
		saveSettings(homeDir+SETTINGS_FILE);
		_gui->updateFullscreen();
	#endif
}

/**
 * @brief Change the resolution.
 *
 * @param go_right true if the resolution should cycle to the right.
 */
void					SceneSettings::_updateResolution(bool go_right) {
	if (go_right) {
		_next_resolution = false;
	}
	else {
		_prev_resolution = false;
	}
	_select_res = go_right ? _select_res + 1 : _select_res - 1;
	if (_select_res < 0) {
		_select_res = 0;
	}
	else if (_select_res >= SceneSettings::nb_resolution) {
		_select_res = SceneSettings::nb_resolution - 1;
	}
	while (SceneSettings::resolutions[_select_res].width > _gui->gameInfo.maxWindowSize.x
	|| SceneSettings::resolutions[_select_res].height > _gui->gameInfo.maxWindowSize.y)
	{
		if (_select_res > 0)
			_select_res--;
		else
			break;
	}
	_current_resolution = SceneSettings::resolutions[_select_res];
	_updateResolutionText();
	#if RESTART_TO_UPDATE_RESOLUTION
		_gui->gameInfo.savedWindowSize.x = _current_resolution.width;
		_gui->gameInfo.savedWindowSize.y = _current_resolution.height;
	#else
		s.j("graphics").i("width") = _current_resolution.width;
		s.j("graphics").i("height") = _current_resolution.height;
		saveSettings(homeDir+SETTINGS_FILE);
		_text_scale = static_cast<float>(_current_resolution.width) * 0.001;
		_gui->udpateDimension();
	#endif
}

/**
 * @brief Reset the keys configurations.
 */
void					SceneSettings::_resetKeys() {
	_reset = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		if (_key_buttons[_input_configuring] != nullptr)
			_key_buttons[_input_configuring]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(_input_configuring)));
	}
	Inputs::resetKeys();
	for (auto i = 0; i < Inputs::nb_input; i++) {
		if (_key_buttons[i] != nullptr)
			_key_buttons[i]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(i)));
	}
}

/**
 * @brief Quit the settings menu and go back to the main menu.
 */
void					SceneSettings::_returnQuit() {
	_return = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getKeyName(static_cast<InputType::Enum>(_input_configuring)));
	}
	SceneManager::loadScene(SceneNames::MAIN_MENU);
}

/**
 * @brief Get the current resolution
 *
 * @return glm::ivec2 the current resolution
 */
glm::ivec2		SceneSettings::getCurResolution() const {
	return glm::ivec2(_current_resolution.width, _current_resolution.height);
}
