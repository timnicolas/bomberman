#include "Sound.hpp"
#include "AudioManager.hpp"
#include "Logging.hpp"

Sound::Sound(std::string filename): _chunk(nullptr), _currents_channels() {
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

Sound::~Sound() {
	stop();
	if (_chunk != nullptr) {
		Mix_FreeChunk(_chunk);
	}
}

/**
	Play the sound at the specified volume.

	@param volume The volume of the sound.

	@throw A SoundException if the sound failed to be played. Not enough channels are alocated.
 */
void										Sound::play(float volume) {
	int			chan;

	if (_chunk != nullptr) {
		chan = Mix_PlayChannel(-1, _chunk, 0);
		if (chan < 0) {
			throw Sound::SoundException(Mix_GetError());
		}
		Mix_Volume(chan, static_cast<int>(volume * MIX_MAX_VOLUME));
		_currents_channels.insert(chan);
	}
}

/**
	Pause all the channels playing this sound.
*/
void										Sound::pause() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_Pause(*it);
	}
}

/**
	Resumes all the channels playing this sound.
*/
void										Sound::resume() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_Resume(*it);
	}
}

/**
	Stop all the channels playing this sound.
*/
void										Sound::stop() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_HaltChannel(*it);
	}
}

/**
	Update the volume for all the channels playing this sound.
	If a channel was set with volume modifier below 1.0, the modifier will be conserved.

	@param volume The new global volume value.
	@param previous_settings The previous global volume value.
*/
void										Sound::updateVolume(float volume, float previous_settings) {
	float		previous_volume;

	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		previous_volume = Mix_Volume(*it, -1);
		Mix_Volume(*it, (previous_volume / previous_settings) * volume);
	}
}

/**
	Remove the specified channel from the list of used channels if found in it.

	@param chan The id of the channel that has just finished playing.

	@return true if the channel was found, false otherwise.
*/
bool										Sound::channelFinished(int chan) {
	if (_currents_channels.find(chan) != _currents_channels.end()) {
		_currents_channels.erase(chan);
		return true;
	}
	return false;
}

Sound::SoundException::SoundException(): std::runtime_error("[SoundException]") {}

Sound::SoundException::SoundException(const char* what_arg) \
	: std::runtime_error(std::string(std::string("[SoundException] ") + what_arg).c_str()) {}

Sound::SoundException::SoundException(const std::string what_arg) \
	: std::runtime_error(std::string(std::string("[SoundException] ") + what_arg).c_str()) {}
