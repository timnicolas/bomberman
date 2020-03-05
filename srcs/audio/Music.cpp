#include "Music.hpp"
#include "AudioManager.hpp"
#include "Logging.hpp"

Music::Music(std::string filename): _mix_mus(nullptr) {
	if (AudioManager::isEnabled()) {
		_mix_mus = Mix_LoadMUS(filename.c_str());
		if (_mix_mus == nullptr) {
			logErr("Failed to load music '" << filename << "'.");
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


void						Music::play(float volume, bool loop) {
	if (_mix_mus != nullptr) {
		if (Mix_PlayMusic(_mix_mus, loop ? -1 : 0) != 0) {
			logErr("Music error: " << Mix_GetError());
			return;
		}
		Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
	}
}
