#include "AudioManager.hpp"
#include "Logging.hpp"
#include "bomberman.hpp"

AudioManager::AudioManager() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		_enabled = false;
		logErr("Failed to init sdl audio.");
	}
	else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		logErr("Failed while loadingaudio mixer: " << Mix_GetError());
		_enabled = false;
	}
	else {
		Mix_AllocateChannels(AudioManager::nb_sound_channels);
		_volume_master = static_cast<float>(s.j("audio").d("masterVolume"));
		_volume_music = static_cast<float>(s.j("audio").d("musicVolume"));
		_volume_sound = static_cast<float>(s.j("audio").d("soundVolume"));
		_enabled = true;
		logInfo("Audio loaded.");
	}
}

AudioManager::~AudioManager() {
	Mix_CloseAudio();
}

AudioManager				&AudioManager::get() {
	static AudioManager		instance;
	return (instance);
}

bool						AudioManager::isEnabled() {
	return AudioManager::get()._enabled;
}

void						AudioManager::updateSettings() {
	AudioManager::get()._updateSettings();
}
void						AudioManager::_updateSettings() {
	_volume_master = static_cast<float>(s.j("audio").d("masterVolume"));
	_volume_music = static_cast<float>(s.j("audio").d("musicVolume"));
	_volume_sound = static_cast<float>(s.j("audio").d("soundVolume"));
}

void						AudioManager::loadMusic(std::string filename) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		inst._loadMusic(filename);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_loadMusic(std::string filename) {
	(void)filename;
}

void						AudioManager::loadSound(std::string filename) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._loadSound(filename);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_loadSound(std::string filename) {
	_sounds[filename] = new Sound(filename);
}

void						AudioManager::playMusic() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._playMusic();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_playMusic() {}

void						AudioManager::pauseMusic() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._pauseMusic();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_pauseMusic() {}

void						AudioManager::playSound(std::string sound_name, float volume) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._playSound(sound_name, volume);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_playSound(std::string sound_name, float volume) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		sound->play(volume * _volume_master * _volume_sound);
	}
	catch (std::out_of_range oor) {
		logErr("Trying to play the sound file '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::unloadSound(std::string sound_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._unloadSound(sound_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_unloadSound(std::string sound_name) {
	try {
		delete _sounds.at(sound_name);
		_sounds.erase(sound_name);
	}
	catch (std::out_of_range oor) {
		logErr("Trying to unload the sound file '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::stopAllSounds() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopAllSounds();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_stopAllSounds() {
	Mix_HaltChannel(-1);
}
