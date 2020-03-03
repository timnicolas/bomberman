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
		Mix_AllocateChannels(AudioManager::nbSoundChannels);
		_volumeMusic = static_cast<float>(s.j("audio").d("musicVolume"));
		_volumeSound = static_cast<float>(s.j("audio").d("soundVolume"));
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
	(void)filename;
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

void						AudioManager::playSound() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._playSound();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_playSound() {}

void						AudioManager::stopSound() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopSound();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_stopSound() {}

void						AudioManager::stopAllSounds() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopAllSounds();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_stopAllSounds() {}
