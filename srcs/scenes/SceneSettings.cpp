#include "SceneSettings.hpp"

SceneSettings::~SceneSettings() {}
SceneSettings::SceneSettings(Gui *gui, float const &dtTime) : SceneMenu(gui, dtTime),
	_current_pane(SettingsType::GRAPHICS),
	_next_resolution(false),
	_prev_resolution(false),
	_update_fullscreen(false)
{
	for (auto i = 0; i < 3; i++) {
		_update_audio[i] = false;
	}
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		_select_pane[i] = false;
		_panes[i] = std::list<ABaseUI*>();
	}
	for (auto i = 0; i < Inputs::nb_input; i++) {
		_update_key[i] = false;
	}
	_fullscreen = s.j("graphics").b("fullscreen");
}
SceneSettings::SceneSettings(SceneSettings const &src) : SceneMenu(src) {
	*this = src;
}

SceneSettings	&SceneSettings::operator=(SceneSettings const &rhs) {
	SceneMenu::operator=(rhs);
	_fullscreen = rhs._fullscreen;
	_current_pane = rhs._current_pane;
	_next_resolution = rhs._next_resolution;
	_prev_resolution = rhs._prev_resolution;
	_update_fullscreen = rhs._update_fullscreen;
	_update_audio[0] = rhs._update_audio[0];
	_update_audio[1] = rhs._update_audio[1];
	_update_audio[2] = rhs._update_audio[2];
	for (auto i = 0; i < SettingsType::nb_types; i++) {
		_select_pane[i] = rhs._select_pane[i];
		_panes[i] = rhs._panes[i];
	}
	for (auto i = 0; i < Inputs::nb_input; i++) {
		_update_key[i] = rhs._update_key[i];
	}
	return *this;
}


bool			SceneSettings::init() {
	glm::vec2 win_size = _gui->gameInfo.windowSize;
	glm::vec2 tmp_pos;
	glm::vec2 tmp_size;
	float menu_width = win_size.x * 0.8;
	float menu_height = win_size.y * 0.9;

	try {
		tmp_size.x = menu_width;
		tmp_size.y = menu_height * 0.2;
		tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
		tmp_pos.y = win_size.y - (win_size.y - menu_height) / 2 - tmp_size.y;
		addText(tmp_pos, tmp_size, "SETTINGS").setTextFont("title");
		tmp_size.y = menu_height * 0.1;
		tmp_size.x = menu_width / 3 * 0.9;
		tmp_pos.x += (menu_width / 3 - tmp_size.x) / 2;
		tmp_pos.y -= tmp_size.y;
		addButton(tmp_pos, tmp_size, "Graphics").addButtonLeftListener(&_select_pane[SettingsType::GRAPHICS]) \
			.setTextAlign(TextAlign::CENTER);
		tmp_pos.x += (menu_width / 3);
		addButton(tmp_pos, tmp_size, "Audio").addButtonLeftListener(&_select_pane[SettingsType::AUDIO]) \
			.setTextAlign(TextAlign::CENTER);
		tmp_pos.x += (menu_width / 3);
		addButton(tmp_pos, tmp_size, "Controls").addButtonLeftListener(&_select_pane[SettingsType::CONTROLS]) \
			.setTextAlign(TextAlign::CENTER);

		/* graphics */
		// TODO(gsmith): add graphics settings UI
		/* audio */
		// TODO(gsmith): add audio settings UI
		/* controls */
		_init_control_pane(tmp_pos, menu_width, menu_height);
	}
	catch (ABaseUI::UIException & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

void			SceneSettings::_init_control_pane(glm::vec2 tmp_pos, float menu_width, float menu_height) {
	glm::vec2 win_size = _gui->gameInfo.windowSize;
	glm::vec2 tmp_size;
	ABaseUI *ptr;

	tmp_size.x = menu_width / 3;
	tmp_size.y = menu_height * 0.09;
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "UP :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::UP]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "DOWN :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::DOWN]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "LEFT :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::LEFT]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "RIGHT :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::RIGHT]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "ACTION :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::ACTION]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "CONFIRM :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::CONFIRM]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x = (win_size.x / 2) - (menu_width / 2);
	tmp_pos.y -= menu_height * 0.1;
	ptr = &addText(tmp_pos, tmp_size, "CANCEL :").setTextAlign(TextAlign::RIGHT);
	_panes[SettingsType::CONTROLS].push_front(ptr);
	tmp_pos.x += (menu_width / 2);
	ptr = &addButton(tmp_pos, tmp_size, "key").addButtonLeftListener(&_update_key[InputType::CANCEL]) \
		.setTextAlign(TextAlign::CENTER);
	_panes[SettingsType::CONTROLS].push_front(ptr);
}
bool			SceneSettings::update() {
	SceneMenu::update();
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
	if (_update_fullscreen) {
		_updateFullscreen();
	}
	if (_confirm) {
		_confirmQuit();
	}
	if (_cancel) {
		_cancelQuit();
	}
	return true;
}

void			SceneSettings::_selectPane(SettingsType::Enum pane_type) {
	_select_pane[pane_type] = false;
	// TODO(gsmith): update which ABaseUI should be displayed.
	logDebug("Switch to pane " << pane_type);
}

void			SceneSettings::_updateKey(InputType::Enum key_type) {
	_update_key[key_type] = false;
	// TODO(gsmith): add button to cancel key configuration ?
	logInfo("Start configuring key...");
	Inputs::configureKey(key_type);
}

void			SceneSettings::_updateFullscreen() {
	_update_fullscreen = false;
	_fullscreen = !_fullscreen;
}

void			SceneSettings::_confirmQuit() {
	_confirm = false;
	// TODO(gsmith): Save settings in json
	_cancelQuit();
}

void			SceneSettings::_cancelQuit() {
	_cancel = false;
	// TODO(gsmith): quit to main menu;
}
