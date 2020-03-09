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
	}
	for (auto i = 0; i < Inputs::nb_input; i++) {
		_update_key[i] = rhs._update_key[i];
	}
	return *this;
}


bool			SceneSettings::init() {
	return true;
}

bool			SceneSettings::update() {
	SceneMenu::update();
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
