#include <utility>
#include "Sound.hpp"
#include "AudioManager.hpp"
#include "Logging.hpp"

/**
 * @brief Construct a new Sound:: Sound object
 *
 * @param filename Sound filename
 */
Sound::Sound(std::string filename): _chunk(nullptr), _currents_channels(), _chan_volume() {
	if (AudioManager::isEnabled()) {
		_chunk = Mix_LoadWAV(filename.c_str());
		if (_chunk == nullptr) {
			throw Sound::SoundException("Failed to load audio sample '" + filename + "'.");
		}
	}
	else {
		logWarn("Trying to load an audio sample but AudioManager is not working.");
	}
}

/**
 * @brief Destroy the Sound:: Sound object
 */
Sound::~Sound() {
	stop();
	if (_chunk != nullptr) {
		Mix_FreeChunk(_chunk);
	}
}

/**
 * @brief Play the sound at the specified volume.
 *
 * @param volume The volume of the sound.
 * @param env_volume The volume settings.
 * @param loop If the sound is on loop.
 * @param fadeIn FadeIn
 * @return int channel used.
 * @throw A SoundException if the sound failed to be played. Not enough channels are allocated.
 */
int										Sound::play(float volume, float env_volume, bool loop, int fadeIn) {
	int			chan;

	if (_chunk != nullptr) {
		chan = Mix_FadeInChannel(-1, _chunk, loop ? -1 : 0, fadeIn);
		if (chan < 0) {
			throw Sound::SoundException(Mix_GetError());
		}
		volume = volume > 1.0f ? 1.0 : volume;
		Mix_Volume(chan, static_cast<int>(volume * env_volume * MIX_MAX_VOLUME));
		_currents_channels.insert(chan);
		_chan_volume.insert(std::pair<int, float>(chan, volume));
		return chan;
	}
	return -2;
}

/**
 * @brief Pause all the channels playing this sound.
 */
void										Sound::pause() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_Pause(*it);
	}
}

/**
 * @brief Resumes all the channels playing this sound.
 */
void										Sound::resume() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_Resume(*it);
	}
}

/**
 * @brief Stop all the channels playing this sound.
 */
void										Sound::stop() {
	std::unordered_set<int>::iterator it = _currents_channels.begin();
	while (it != _currents_channels.end()) {
		Mix_HaltChannel(*it);  // Sound::channelFinished() as callback delete <it>
		it = _currents_channels.begin();
	}
}

/**
 * @brief Update the volume for all the channels playing this sound.
 * If a channel was set with volume modifier below 1.0, the modifier will be
 * conserved.
 *
 * @param volume The new global volume value.
 */
void										Sound::updateVolume(float volume) {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_Volume(*it, _chan_volume.at(*it) * volume * MIX_MAX_VOLUME);
	}
}

/**
 * @brief Remove the specified channel from the list of used channels if found in it.
 *
 * @param chan The id of the channel that has just finished playing.
 * @return true if the channel was found.
 * @return false otherwise.
 */
bool										Sound::channelFinished(int chan) {
	if (_currents_channels.find(chan) != _currents_channels.end()) {
		_currents_channels.erase(chan);
		_chan_volume.erase(chan);
		return true;
	}
	return false;
}

/**
 * @brief Construct a new Sound:: Sound Exception:: Sound Exception object
 */
Sound::SoundException::SoundException(): std::runtime_error("[SoundException]") {}

/**
 * @brief Construct a new Sound:: Sound Exception:: Sound Exception object
 *
 * @param what_arg Error message
 */
Sound::SoundException::SoundException(const char* what_arg) \
	: std::runtime_error(std::string(std::string("[SoundException] ") + what_arg).c_str()) {}

/**
 * @brief Construct a new Sound:: Sound Exception:: Sound Exception object
 *
 * @param what_arg Error message
 */
Sound::SoundException::SoundException(const std::string what_arg) \
	: std::runtime_error(std::string(std::string("[SoundException] ") + what_arg).c_str()) {}
