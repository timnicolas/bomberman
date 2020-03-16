#include "AudioManager.hpp"
#include <unistd.h>
#if __linux__
# include <linux/limits.h>
#endif
#include "Logging.hpp"
#include "bomberman.hpp"

std::string					AudioManager::_assets_path = std::string();

AudioManager::AudioManager(): _music_modifier(1.0) {
	char	*path = getcwd(NULL, PATH_MAX);
	AudioManager::_assets_path = std::string(path);
	AudioManager::_assets_path += "/bomberman-assets/";
	free(path);
	logDebug("Assets_path: " << AudioManager::_assets_path);
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		_enabled = false;
		logErr("Failed to init sdl audio.");
	}
	else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		_enabled = false;
		logErr("Failed while loading audio mixer: " << Mix_GetError());
	}
	else {
		Mix_AllocateChannels(AudioManager::nb_sound_channels);
		Mix_ChannelFinished(&AudioManager::_channelHalted);
		_volume_master = static_cast<float>(s.j("audio").d("Master volume"));
		_volume_music = static_cast<float>(s.j("audio").d("Music volume"));
		_volume_sound = static_cast<float>(s.j("audio").d("Sound volume"));
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

/**
	Return a reference to the singleton AudioManager.

	@return the reference to the singleton.
*/
AudioManager				&AudioManager::get() {
	static AudioManager		instance;
	return (instance);
}

/**
	Indicate if the AudioManager has been correctly initialized.

	@return true if the AudioManager is working.
*/
bool						AudioManager::isEnabled() {
	return AudioManager::get()._enabled;
}
/**
 	Change the volume for the music and all the sound being played.
	If the music was set with a volume modifier lower than 1.0, it will be conserved.
*/
void						AudioManager::updateSettings() {
	AudioManager::get()._updateSettings();
}
void						AudioManager::_updateSettings() {
	_volume_master = static_cast<float>(s.j("audio").d("Master volume"));
	_volume_music = static_cast<float>(s.j("audio").d("Music volume"));
	_volume_sound = static_cast<float>(s.j("audio").d("Sound volume"));
	Mix_VolumeMusic(_music_modifier * MIX_MAX_VOLUME * _volume_master * _volume_music);
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		it->second->updateVolume(_volume_master * _volume_sound);
	}
}


/**
	Load a new music from its file name.

	@param file_name The path to the music file, starting from the asset directory.

	@throw A MusicException if the music failed to be loaded. The path might be incorrect.
*/
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
	_musics[file_name] = new Music(AudioManager::_assets_path + file_name);
}

/**
	Play a music with a volume modifier.

	@param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.
	@param loop Indicate if the music should loop. false by default.

	@throw A MusicException if the music failed to be played.
*/
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
		volume = volume > 1.0 ? 1.0 : volume;
		music->play(volume * _volume_master * _volume_music, loop);
		_music_modifier = volume;
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to play the music '" << music_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Pause the music currently played.
*/
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

/**
	Resume the music currently played.
*/
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

/**
	Stop the music currently played.
*/
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

/**
	Unload the music from the audio manager.

	@param file_name The path to the music file, starting from the asset directory.
*/
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
	catch (std::out_of_range const &oor) {
		logErr("Trying to unload the music '" << music_name << "' but it has not been loaded.");
		return;
	}
}


/**
	Load a new sound from its file name.

	@param file_name The path to the sound file, starting from the asset directory.

	@throw A SoundException if the sound failed to be loaded. The path might be incorrect.
*/
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
	_sounds[file_name] = new Sound(AudioManager::_assets_path + file_name);
}

/**
	Play a sound identified by file name with a volume modifier.

	@param sound_name the file name of the sound.
	@param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.

	@throw A SoundException if the sound failed to be played.
*/
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
		volume = volume > 1.0 ? 1.0 : volume;
		sound->play(volume,  _volume_master * _volume_sound);
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to play the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Pause all the channels of the sound identified by file name.

	@param sound_name the file name of the sound.
*/
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
	catch (std::out_of_range const &oor) {
		logErr("Trying to pause the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Resume all the channels of the sound identified by file name.

	@param sound_name the file name of the sound.
*/
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
	catch (std::out_of_range const &oor) {
		logErr("Trying to resume the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Stop all the channels of the sound identified by file name.

	@param sound_name the file name of the sound.
*/
void						AudioManager::stopSound(std::string sound_name) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopSound(sound_name);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
void						AudioManager::_stopSound(std::string sound_name) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		sound->stop();
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to stop the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Pause all the channels.
*/
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

/**
	Resume all the channels.
*/
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

/**
	Unload the sound identified by file name.

	@param sound_name the file name of the sound.
*/
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
	catch (std::out_of_range const &oor) {
		logErr("Trying to unload the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
	Stop all the channels.
*/
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
