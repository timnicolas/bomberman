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
		Mix_ChannelFinished(&AudioManager::_channelHalted);
		_volume_master = static_cast<float>(s.j("audio").d("masterVolume"));
		_volume_music = static_cast<float>(s.j("audio").d("musicVolume"));
		_volume_sound = static_cast<float>(s.j("audio").d("soundVolume"));
		_enabled = true;
		logInfo("Audio loaded.");
	}
}

AudioManager::~AudioManager() {
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		delete it->second;
	}
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
	float setting_sound = _volume_master * _volume_sound;
	// float setting_music = _volume_master * _volume_music;
	_volume_master = static_cast<float>(s.j("audio").d("masterVolume"));
	_volume_music = static_cast<float>(s.j("audio").d("musicVolume"));
	_volume_sound = static_cast<float>(s.j("audio").d("soundVolume"));
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		it->second->updateVolume(_volume_master * _volume_sound, setting_sound);
	}
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
		logErr("Trying to play the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::pauseSound(std::string sound_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._pauseSound(sound_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_pauseSound(std::string sound_name) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		sound->pause();
	}
	catch (std::out_of_range oor) {
		logErr("Trying to pause the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::resumeSound(std::string sound_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._resumeSound(sound_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_resumeSound(std::string sound_name) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		sound->resume();
	}
	catch (std::out_of_range oor) {
		logErr("Trying to resume the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::pauseAllSounds() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._pauseAllSounds();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_pauseAllSounds() {
	Mix_Pause(-1);
}

void						AudioManager::resumeAllSounds() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._resumeAllSounds();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_resumeAllSounds() {
	Mix_Resume(-1);
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

void						AudioManager::_channelHalted(int chan) {
	AudioManager	&inst = AudioManager::get();
	for (auto it = inst._sounds.begin(); it != inst._sounds.end(); it++) {
		it->second->channelFinished(chan);
	}
}
