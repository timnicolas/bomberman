#include "SceneSettings.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"

SceneSettings::~SceneSettings() {}
SceneSettings::SceneSettings(Gui *gui, float const &dtTime) : SceneMenu(gui, dtTime),
	_input_configuring(-1),
	_current_pane(SettingsType::GRAPHICS),
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
	_fullscreen_button = nullptr;
	_fullscreen = s.j("graphics").b("fullscreen");
	int width = s.j("graphics").i("width");
	int height = s.j("graphics").i("height");
	_current_resolution = {
		width,
		height
	};
	_text_scale = static_cast<float>(width) * 0.001;
	logDebug(_text_scale);
}
SceneSettings::SceneSettings(SceneSettings const &src) : SceneMenu(src) {
	*this = src;
}

SceneSettings			&SceneSettings::operator=(SceneSettings const &rhs) {
	SceneMenu::operator=(rhs);
	_input_configuring = rhs._input_configuring;
	_fullscreen = rhs._fullscreen;
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
	_fullscreen_button = rhs._fullscreen_button;
	_text_scale = rhs._text_scale;
	return *this;
}
SceneSettings::res		SceneSettings::resolutions[SceneSettings::nb_resolution] = {
	{800, 600},
	{1200, 600},
	{1600, 900},
	{1920, 1080}
};

const std::string		SceneSettings::audio_name[3] = {
	"Master volume",
	"Music volume",
	"Sound volume",
};

bool					SceneSettings::init() {
	glm::vec2 win_size = _gui->gameInfo.windowSize;
	glm::vec2 tmp_pos;
	glm::vec2 tmp_size;
	float menu_width = win_size.x * 0.8;
	float menu_height = win_size.y * 0.9;

	try {
		tmp_size.y = menu_height * 0.1;
		tmp_size.x = tmp_size.y;
		tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
		tmp_pos.y = win_size.y - (win_size.y - menu_height) / 2 - tmp_size.y;
		addButton(tmp_pos, tmp_size, "X").addButtonLeftListener(&_return) \
			.setTextScale(_text_scale).setTextAlign(TextAlign::CENTER);
		tmp_size.x = menu_width;
		tmp_size.y = menu_height * 0.2;
		tmp_pos.y = win_size.y - (win_size.y - menu_height) / 2 - tmp_size.y;
		addText(tmp_pos, tmp_size, "SETTINGS").setTextFont("title");
		tmp_size.y = menu_height * 0.1;
		tmp_size.x = menu_width / 3 * 0.9;
		tmp_pos.x += (menu_width / 3 - tmp_size.x) / 2;
		tmp_pos.y -= tmp_size.y;
		addButton(tmp_pos, tmp_size, "Graphics").addButtonLeftListener(&_select_pane[SettingsType::GRAPHICS]) \
			.setTextScale(_text_scale).setTextAlign(TextAlign::CENTER);
		tmp_pos.x += (menu_width / 3);
		addButton(tmp_pos, tmp_size, "Audio").addButtonLeftListener(&_select_pane[SettingsType::AUDIO]) \
			.setTextScale(_text_scale).setTextAlign(TextAlign::CENTER);
		tmp_pos.x += (menu_width / 3);
		addButton(tmp_pos, tmp_size, "Controls").addButtonLeftListener(&_select_pane[SettingsType::CONTROLS]) \
			.setTextScale(_text_scale).setTextAlign(TextAlign::CENTER);

		/* graphics */
		_init_graphics_pane(tmp_pos, menu_width, menu_height);
		/* audio */
		_init_audio_pane(tmp_pos, menu_width, menu_height);
		/* controls */
		_init_control_pane(tmp_pos, menu_width, menu_height);
		AudioManager::loadSound("sounds/bell.ogg");
		AudioManager::loadMusic("sounds/puzzle.ogg");
		AudioManager::playMusic("sounds/puzzle.ogg", 1.0f, true);
	}
	catch (ABaseUI::UIException & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

void					SceneSettings::_init_graphics_pane(glm::vec2 tmp_pos, float menu_width, float menu_height) {
	glm::vec2	win_size = _gui->gameInfo.windowSize;
	glm::vec2	tmp_size;
	ABaseUI		*ptr;

	tmp_size.y = menu_height * 0.2;
	tmp_pos.y -= menu_height * 0.2;
	tmp_size.x = menu_width;
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	std::string warning = "Modifications on the graphical settings will be applied after the game restarts.";
	ptr = &addText(tmp_pos, tmp_size, warning).setTextAlign(TextAlign::CENTER) \
		.setTextScale(_text_scale).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);

	tmp_size.y = menu_height * 0.1;
	tmp_size.x = menu_width / 3;
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "Fullscreen :").setTextAlign(TextAlign::RIGHT) \
		.setTextScale(_text_scale).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmp_size.x = menu_width / 10;
	tmp_pos.x += menu_width / 2 + menu_width / 6 - tmp_size.x / 2;
	_fullscreen_button = &addButton(tmp_pos, tmp_size, "OFF");
	ptr = &_fullscreen_button->addButtonLeftListener(&_update_fullscreen).setTextScale(_text_scale) \
		.setTextAlign(TextAlign::CENTER);
	_updateFullscreenButton();
	_panes[SettingsType::GRAPHICS].push_front(ptr);

	tmp_size.x = menu_width / 3;
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.3;
	ptr = &addText(tmp_pos, tmp_size, "Resolution :").setTextAlign(TextAlign::RIGHT) \
		.setTextScale(_text_scale).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	_resolution_text = &addText(tmp_pos, tmp_size, "800x600");
	ptr = &_resolution_text->setTextAlign(TextAlign::CENTER) \
		.setTextScale(_text_scale).setEnabled(true);
	_updateResolutionText();
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmp_size.x = menu_width / 10;
	tmp_pos.x -= tmp_size.x;
	ptr = &addButton(tmp_pos, tmp_size, "<").addButtonLeftListener(&_prev_resolution) \
		.setTextScale(_text_scale).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
	tmp_pos.x += menu_width / 3 + tmp_size.x;
	ptr = &addButton(tmp_pos, tmp_size, ">").addButtonLeftListener(&_next_resolution) \
		.setTextScale(_text_scale).setEnabled(true);
	_panes[SettingsType::GRAPHICS].push_front(ptr);
}

void					SceneSettings::_init_audio_pane(glm::vec2 tmp_pos, float menu_width, float menu_height) {
	glm::vec2	win_size = _gui->gameInfo.windowSize;
	glm::vec2	tmp_size;
	ABaseUI		*ptr;
	float		tmp_val;

	tmp_size.y = menu_height * 0.1;
	for (auto i = 0; i < 3; i++) {
		tmp_size.x = menu_width / 3;
		tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
		tmp_pos.y -= menu_height * 0.2;
		ptr = &addText(tmp_pos, tmp_size,		 SceneSettings::audio_name[i] + " :").setTextAlign(TextAlign::RIGHT) \
			.setTextScale(_text_scale).setEnabled(false);
		_panes[SettingsType::AUDIO].push_front(ptr);
		tmp_size.x *= 2;
		tmp_pos.x += (menu_width / 3);
		tmp_val = s.j("audio").d(SceneSettings::audio_name[i]);
		ptr = &addSlider(tmp_pos, tmp_size, 0, 100, tmp_val * 100, 1).addSliderListener(&_update_audio[i]) \
			.addButtonLeftListener(&_save_audio[i]).setTextScale(_text_scale).setEnabled(false);
		_panes[SettingsType::AUDIO].push_front(ptr);
	}
}

void					SceneSettings::_init_control_pane(glm::vec2 tmp_pos, float menu_width, float menu_height) {
	glm::vec2 win_size = _gui->gameInfo.windowSize;
	glm::vec2 tmp_size;
	ABaseUI *ptr;
	std::string key_name;

	tmp_size.x = menu_width / 3;
	tmp_size.y = menu_height * 0.09;
	for (auto i = 0; i < Inputs::nb_input; i++) {
		tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
		tmp_pos.y -= menu_height * 0.1;
		ptr = &addText(tmp_pos, tmp_size, Inputs::input_type_name[i] + " :").setTextAlign(TextAlign::RIGHT) \
			.setTextScale(_text_scale).setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
		tmp_pos.x += (menu_width / 2);
		key_name = Inputs::getInputKeyName(static_cast<InputType::Enum>(i));
		ptr = &addButton(tmp_pos, tmp_size, key_name).addButtonLeftListener(&_update_key[i]) \
			.setTextAlign(TextAlign::CENTER).setTextScale(_text_scale).setEnabled(false);
		_panes[SettingsType::CONTROLS].push_front(ptr);
		_key_buttons[i] = reinterpret_cast<ButtonUI*>(ptr);
	}
}

void					SceneSettings::_updateResolutionText() {
	std::string		text = std::to_string(_current_resolution.width) + "x" \
		+ std::to_string(_current_resolution.height);
	_resolution_text->setText(text);
}

void					SceneSettings::_updateFullscreenButton() {
	if (_fullscreen_button != nullptr) {
		std::string symbol;
		glm::vec4 color;
		if (_fullscreen) {
			symbol = "ON";
			color = glm::vec4(0.2, 0.8, 0.2, 1.0);
		}
		else {
			symbol = "OFF";
			color = glm::vec4(0.8, 0.2, 0.2, 1.0);
		}
		_fullscreen_button->setText(symbol).setTextColor(color);
	}
}

bool					SceneSettings::update() {
	SceneMenu::update();
	if (_input_configuring >= 0 && !Inputs::isConfiguring()) {
		_key_buttons[_input_configuring]->setText(Inputs::getInputKeyName(static_cast<InputType::Enum>(_input_configuring)));
		_input_configuring = -1;
	}
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
	if (_update_fullscreen) {
		_updateFullscreen();
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
	if (Inputs::getKeyDown(InputType::ACTION)) {
		try {
			AudioManager::playSound("sounds/bell.ogg");
		}
		catch (Sound::SoundException e) {
			logErr(e.what());
		}
	}
	return true;
}

void					SceneSettings::_selectPane(SettingsType::Enum pane_type) {
	_select_pane[pane_type] = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getInputKeyName(static_cast<InputType::Enum>(_input_configuring)));
		_input_configuring = -1;
	}
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		for (auto it = _panes[i].begin(); it != _panes[i].end(); it++) {
			(*it)->setEnabled(i == pane_type);
		}
	}
}

void					SceneSettings::_updateKey(InputType::Enum key_type) {
	_update_key[key_type] = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getInputKeyName(static_cast<InputType::Enum>(_input_configuring)));
	}
	logInfo("Start configuring key...");
	_key_buttons[key_type]->setText("...");
	_input_configuring = key_type;
	Inputs::configureKey(key_type);
}

void					SceneSettings::_updateAudioVolume(int audio_index) {
	std::string volume_name =		 SceneSettings::audio_name[audio_index];
	float volume = _update_audio[audio_index];
	_audio_volume[audio_index] = _update_audio[audio_index];
	s.j("audio").d(volume_name) = volume / 100;
	AudioManager::updateSettings();
}

void					SceneSettings::_saveAudioVolume(int audio_index) {
	_save_audio[audio_index] = false;
	s.saveToFile("configs/settings.json");
}

void					SceneSettings::_updateFullscreen() {
	_update_fullscreen = false;
	_fullscreen = !_fullscreen;
	_updateFullscreenButton();
	s.j("graphics").b("fullscreen") = _fullscreen;
	s.saveToFile("configs/settings.json");
}

void					SceneSettings::_updateResolution(bool go_right) {
	if (go_right) {
		_next_resolution = false;
	}
	else {
		_prev_resolution = false;
	}
	// TODO(gsmith): actually change the resolution.
	logDebug("Change resolution.");
}

void					SceneSettings::_returnQuit() {
	_return = false;
	if (_input_configuring >= 0) {
		Inputs::cancelConfiguration();
		_key_buttons[_input_configuring]->setText(Inputs::getInputKeyName(static_cast<InputType::Enum>(_input_configuring)));
	}
	SceneManager::loadScene(SceneNames::MAIN_MENU);
}
