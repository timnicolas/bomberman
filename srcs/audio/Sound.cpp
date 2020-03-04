#include "Sound.hpp"
#include "AudioManager.hpp"
#include "Logging.hpp"

Sound::Sound(std::string filename): _filename(filename), _chunk(nullptr), _currents_channels() {
	if (AudioManager::isEnabled()) {
		_chunk = Mix_LoadWAV(filename.c_str());
		if (_chunk == nullptr) {
			logErr("Failed to load audio sample '" << filename << "'.");
		}
	}
	else {
		logWarn("Trying to load an audio sample but AudioManager is not working.");
	}
}

Sound::~Sound() {
	this->stop();
	if (_chunk != nullptr) {
		Mix_FreeChunk(_chunk);
	}
}

void										Sound::play(float volume) {
	int			chan;

	chan = Mix_PlayChannel(-1, _chunk, 0);
	if (chan < 0) {
		logErr("Sound error: " << Mix_GetError());
		return;
	}
	Mix_Volume(chan, static_cast<int>(volume * MIX_MAX_VOLUME));
	_currents_channels.insert(chan);
}

void										Sound::stop() {
	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		Mix_HaltChannel(*it);
	}
}

void										Sound::updateVolume(float volume, float previous_settings) {
	float		previous_volume;

	for (auto it = _currents_channels.begin(); it != _currents_channels.end(); it++) {
		previous_volume = Mix_Volume(*it, -1);
		Mix_Volume(*it, (previous_volume / MIX_MAX_VOLUME / previous_settings) * volume);
	}
}

bool										Sound::channelFinished(int chan) {
	if (_currents_channels.find(chan) != _currents_channels.end()) {
		_currents_channels.erase(chan);
		return true;
	}
	return false;
}
