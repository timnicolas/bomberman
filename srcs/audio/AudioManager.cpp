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
	_stopMusic();
	for (auto it = _musics.begin(); it != _musics.end(); it++) {
		delete it->second;
	}
	_stopAllSounds();
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
	float previous_music = Mix_VolumeMusic(-1) / (MIX_MAX_VOLUME * _volume_master * _volume_music);
	_volume_master = static_cast<float>(s.j("audio").d("masterVolume"));
	_volume_music = static_cast<float>(s.j("audio").d("musicVolume"));
	_volume_sound = static_cast<float>(s.j("audio").d("soundVolume"));
	Mix_VolumeMusic(previous_music * _volume_master * _volume_music);
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		it->second->updateVolume(_volume_master * _volume_sound, setting_sound);
	}
}

void						AudioManager::loadMusic(std::string file_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		inst._loadMusic(file_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_loadMusic(std::string file_name) {
	_musics[file_name] = new Music(file_name);
}

void						AudioManager::playMusic(std::string music_name, float volume, bool loop) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._playMusic(music_name, volume, loop);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_playMusic(std::string music_name, float volume, bool loop) {
	try {
		Music	*music = _musics.at(music_name);
		music->play(volume * _volume_master * _volume_sound, loop);
	}
	catch (std::out_of_range oor) {
		logErr("Trying to play the music '" << music_name << "' but it has not been loaded.");
		return;
	}
}

void						AudioManager::pauseMusic() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._pauseMusic();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_pauseMusic() {
	Mix_PauseMusic();
}

void						AudioManager::resumeMusic() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._resumeMusic();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_resumeMusic() {
	Mix_ResumeMusic();
}

void						AudioManager::stopMusic() {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopMusic();
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_stopMusic() {
	Mix_HaltMusic();
}

void						AudioManager::unloadMusic(std::string music_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._unloadMusic(music_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_unloadMusic(std::string music_name) {
	try {
		delete _musics.at(music_name);
		_musics.erase(music_name);
	}
	catch (std::out_of_range oor) {
		logErr("Trying to unload the music '" << music_name << "' but it has not been loaded.");
		return;
	}
}


void						AudioManager::loadSound(std::string file_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._loadSound(file_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_loadSound(std::string file_name) {
	_sounds[file_name] = new Sound(file_name);
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
		logErr("Trying to unload the sound '" << sound_name << "' but it has not been loaded.");
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
