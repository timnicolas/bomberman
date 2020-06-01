#include "AudioManager.hpp"
#include <unistd.h>
#if __linux__
# include <linux/limits.h>
#endif
#include "Logging.hpp"
#include "bomberman.hpp"

// -- Static members -----------------------------------------------------------

std::string					AudioManager::_assets_path = std::string();

std::vector<std::string> AudioManager::musics = {
	"sounds/french79-between_the_buttons.ogg",
	"sounds/french79-naked_city.ogg",
	"sounds/superpoze-siver_head.ogg",
	"sounds/the_blaze-territory.ogg",
	"sounds/eirik_suhrke-a_new_morning.ogg",
	"sounds/moon-crystals.ogg",
	"sounds/moon-hydrogen.ogg"
};

// -- Methods ------------------------------------------------------------------

/**
 * @brief Construct a new Audio Manager:: Audio Manager object
 */
AudioManager::AudioManager(): _music_modifier(1.0) {
	char	*path = getcwd(NULL, PATH_MAX);
	AudioManager::_assets_path = std::string(path);
	AudioManager::_assets_path += "/bomberman-assets/";
	_musicPaused = false;
	free(path);
	// logDebug("Assets_path: " << AudioManager::_assets_path);
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
		_volume_master = static_cast<float>(s.j("audio").d("master"));
		_volume_music = static_cast<float>(s.j("audio").d("music"));
		_volume_sound = static_cast<float>(s.j("audio").d("sound"));
		_enabled = true;
		logDebug("Audio loaded.");
	}
}

/**
 * @brief Destroy the Audio Manager:: Audio Manager object
 */
AudioManager::~AudioManager() {
	_stopMusic();
	for (auto it = _musics.begin(); it != _musics.end(); it++) {
		delete it->second;
	}
	_musics.clear();
	_stopAllSounds();
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		delete it->second;
	}
	_sounds.clear();
	Mix_CloseAudio();
}

/**
 * @brief Return a reference to the singleton AudioManager.
 *
 * @return AudioManager& the reference to the singleton.
 */
AudioManager				&AudioManager::get() {
	static AudioManager		instance;
	return (instance);
}

/**
 * @brief Indicate if the AudioManager has been correctly initialized.
 *
 * @return true if the AudioManager is working.
 * @return false otherwise.
 */
bool						AudioManager::isEnabled() {
	return AudioManager::get()._enabled;
}

/**
 * @brief Change the volume for the music and all the sound being played.
 * If the music was set with a volume modifier lower than 1.0, it will be conserved.
 */
void						AudioManager::updateSettings() {
	AudioManager::get()._updateSettings();
}
void						AudioManager::_updateSettings() {
	_volume_master = static_cast<float>(s.j("audio").d("master"));
	_volume_music = static_cast<float>(s.j("audio").d("music"));
	_volume_sound = static_cast<float>(s.j("audio").d("sound"));
	Mix_VolumeMusic(_music_modifier * MIX_MAX_VOLUME * _volume_master * _volume_music);
	for (auto it = _sounds.begin(); it != _sounds.end(); it++) {
		it->second->updateVolume(_volume_master * _volume_sound);
	}
}

/**
 * @brief Load a new music from its file name.
 *
 * @param file_name The path to the music file, starting from the asset directory.
 * @param silent Don't show warning if enabled
 *
 * @throw A MusicException if the music failed to be loaded. The path might be incorrect.
 */
void						AudioManager::loadMusic(std::string file_name, bool silent) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		inst._loadMusic(file_name, silent);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
/**
 * @brief Load a new music from its file name.
 *
 * @param file_name The path to the music file, starting from the asset directory.
 * @param silent Don't show warning if enabled
 *
 * @throw A MusicException if the music failed to be loaded. The path might be incorrect.
 */
void						AudioManager::_loadMusic(std::string file_name, bool silent) {
	if (_musics.find(file_name) != _musics.end()) {
		if (!silent)
			logWarn("Music " << file_name << " already loaded");
		return;
	}
	_musics[file_name] = new Music(AudioManager::_assets_path + file_name);
}

/**
 * @brief Play a music with a volume modifier.
 *
 * @param music_name The name of the file of the music we want to play.
 * @param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.
 * @param loop Indicate if the music should loop. false by default.
 *
 * @throw A MusicException if the music failed to be played.
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
/**
 * @brief Play a music with a volume modifier.
 *
 * @param music_name The name of the file of the music we want to play.
 * @param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.
 * @param loop Indicate if the music should loop. false by default.
 *
 * @throw A MusicException if the music failed to be played.
 */
void						AudioManager::_playMusic(std::string music_name, float volume, bool loop) {
	try {
		Music	*music = _musics.at(music_name);
		volume = volume > 1.0 ? 1.0 : volume;
		music->play(volume * _volume_master * _volume_music, loop);
		_music_modifier = volume;
		_soundsMutesMusic.clear();
		if (_musicPaused) {
			_pauseMusic();
		}
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to play the music '" << music_name << "' but it has not been loaded.");
		return;
	}
}

/**
 * @brief Pause the music currently played.
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
/**
 * @brief Pause the music currently played.
 */
void						AudioManager::_pauseMusic() {
	Mix_PauseMusic();
	_musicPaused = true;
}

/**
 * @brief Resume the music currently played.
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
/**
 * @brief Resume the music currently played.
 */
void						AudioManager::_resumeMusic() {
	Mix_ResumeMusic();
	_musicPaused = false;
}

/**
 * @brief Stop the music currently played.
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
/**
 * @brief Stop the music currently played.
 */
void						AudioManager::_stopMusic() {
	Mix_FadeOutMusic(2000);
}

/**
 * @brief Unload the music from the audio manager.
 *
 * @param music_name The path to the music file, starting from the asset directory.
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
/**
 * @brief Unload the music from the audio manager.
 *
 * @param music_name The path to the music file, starting from the asset directory.
 */
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
 * @brief Load a new sound from its file name.
 *
 * @param file_name The path to the sound file, starting from the asset directory.
 * @param silent Don't show warning if enabled
 *
 * @throw A SoundException if the sound failed to be loaded. The path might be incorrect.
 */
void						AudioManager::loadSound(std::string file_name, bool silent) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._loadSound(file_name, silent);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
/**
 * @brief Load a new sound from its file name.
 *
 * @param file_name The path to the sound file, starting from the asset directory.
 * @param silent Don't show warning if enabled
 *
 * @throw A SoundException if the sound failed to be loaded. The path might be incorrect.
 */
void						AudioManager::_loadSound(std::string file_name, bool silent) {
	if (_sounds.find(file_name) != _sounds.end()) {
		if (!silent)
			logWarn("Sound " << file_name << " already loaded");
		return;
	}
	_sounds[file_name] = new Sound(AudioManager::_assets_path + file_name);
}

/**
 * @brief Play a sound identified by file name with a volume modifier.
 *
 * @param sound_name the file name of the sound.
 * @param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.
 * @param loop If the sound is played on loop.
 * @param muteMusic If we need to mute the music during the sound.
 * @param fadeIn FadeIn
 *
 * @throw A SoundException if the sound failed to be played.
 */
void						AudioManager::playSound(std::string sound_name, float volume, bool loop, bool muteMusic, int fadeIn) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._playSound(sound_name, volume, loop, muteMusic, fadeIn);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
/**
 * @brief Play a sound identified by file name with a volume modifier.
 *
 * @param sound_name the file name of the sound.
 * @param volume The volume modifier apply on top of the audio setting. Set to 1.0 by default.
 * @param loop If the sound is played on loop.
 * @param muteMusic If we need to mute the music during the sound.
 * @param fadeIn FadeIn
 *
 * @throw A SoundException if the sound failed to be played.
 */
void						AudioManager::_playSound(std::string sound_name, float volume, bool loop, bool muteMusic, int fadeIn) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		volume = volume > 1.0 ? 1.0 : volume;
		int chan = sound->play(volume, _volume_master * _volume_sound, loop, fadeIn);
		if (muteMusic) {
			pauseMusic();
			_soundsMutesMusic.insert(chan);
		}
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to play the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
 * @brief Pause all the channels of the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
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
/**
 * @brief Pause all the channels of the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
 */
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
/**
	Resume all the channels of the sound identified by file name.

	@param sound_name the file name of the sound.
*/
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
 * @brief Stop all the channels of the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
 * @param fadeOut the fadeOut effect.
 */
void						AudioManager::stopSound(std::string sound_name, int fadeOut) {
	AudioManager &inst = AudioManager::get();
	if (inst._enabled) {
		AudioManager::get()._stopSound(sound_name, fadeOut);
	}
	else {
		logWarn("AudioManager is not enabled.");
	}
}
/**
 * @brief Stop all the channels of the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
 * @param fadeOut the fadeOut effect.
 */
void						AudioManager::_stopSound(std::string sound_name, int fadeOut) {
	try {
		Sound	*sound = _sounds.at(sound_name);
		if ((_volume_master * _volume_sound) == 0)
			fadeOut = 0;
		sound->stop(fadeOut);
	}
	catch (std::out_of_range const &oor) {
		logErr("Trying to stop the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
 * @brief Pause all the channels.
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
/**
 * @brief Pause all the channels.
 */
void						AudioManager::_pauseAllSounds() {
	Mix_Pause(-1);
}

/**
 * @brief Resume all the channels.
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
/**
 * @brief Resume all the channels.
 */
void						AudioManager::_resumeAllSounds() {
	Mix_Resume(-1);
}

/**
 * @brief Unload the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
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
/**
 * @brief Unload the sound identified by file name.
 *
 * @param sound_name the file name of the sound.
 */
void						AudioManager::_unloadSound(std::string sound_name) {
	try {
		delete _sounds.at(sound_name);
		_sounds.erase(sound_name);
	}
	catch (std::out_of_range const &oor) {
		logWarn("Trying to unload the sound '" << sound_name << "' but it has not been loaded.");
		return;
	}
}

/**
 * @brief Stop all the channels.
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
/**
 * @brief Stop all the channels.
 */
void						AudioManager::_stopAllSounds() {
	Mix_HaltChannel(-1);
}

/**
 * @brief Callback called when a music or sound finish.
 *
 * @param chan channel used by the sound.
 */
void						AudioManager::_channelHalted(int chan) {
	AudioManager	&inst = AudioManager::get();
	std::unordered_set<int>::const_iterator got = inst._soundsMutesMusic.find(chan);
	if (got != inst._soundsMutesMusic.end())
		inst._soundsMutesMusic.erase(chan);
	if (inst._musicPaused) {
		if (!inst._soundsMutesMusic.size()) {
			resumeMusic();
		}
	}
	for (auto it = inst._sounds.begin(); it != inst._sounds.end(); it++) {
		it->second->channelFinished(chan);
	}
}
