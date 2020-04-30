#ifndef SOUND_HPP
# define SOUND_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <string>
# include <unordered_set>
# include <map>
#include <stdexcept>

/**
 * @brief Sound class for AudioManager
 */
class Sound {
public:
	class SoundException: public std::runtime_error {
		public:
			SoundException();
			explicit SoundException(const char* what_arg);
			explicit SoundException(const std::string what_arg);
	};

	explicit Sound(std::string filename);
	~Sound();

	int											play(float volume, float env_volume, bool loop = false);
	void										pause();
	void										resume();
	void										stop();
	void										updateVolume(float volume);
	bool										channelFinished(int chan);

private:
	Sound();
	Sound(const Sound &src);
	Sound										&operator=(const Sound &src);

	Mix_Chunk *									_chunk;
	std::unordered_set<int>						_currents_channels;
	std::map<int, float>						_chan_volume;
};

#endif
