#include "Music.hpp"
#include "AudioManager.hpp"
#include "Logging.hpp"

Music::Music(std::string filename): _mix_mus(nullptr) {
	if (AudioManager::isEnabled()) {
		_mix_mus = Mix_LoadMUS(filename.c_str());
		if (_mix_mus == nullptr) {
			throw Music::MusicException("Failed to load music '" + filename + "'.");
		}
	}
	else {
		logWarn("Trying to load a music but AudioManager is not working.");
	}
}

Music::~Music() {
	if (_mix_mus != nullptr) {
		Mix_FreeMusic(_mix_mus);
	}
}

/**
	Play the music at the specified volume.

	@param volume The volume of the music.
	@param loop Specify if the music should loop, false by default.

	@throw A MusicException if the music failed to be played.
 */
void						Music::play(float volume, bool loop) {
	if (_mix_mus != nullptr) {
		if (Mix_PlayMusic(_mix_mus, loop ? -1 : 0) != 0) {
			throw Music::MusicException(Mix_GetError());
		}
		Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
	}
}

Music::MusicException::MusicException(): std::runtime_error("[MusicException]") {}

Music::MusicException::MusicException(const char* what_arg) \
	: std::runtime_error(std::string(std::string("[MusicException] ") + what_arg).c_str()) {}

Music::MusicException::MusicException(const std::string what_arg) \
	: std::runtime_error(std::string(std::string("[MusicException] ") + what_arg).c_str()) {}
