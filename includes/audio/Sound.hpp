#ifndef SOUND_HPP
# define SOUND_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <string>
# include <unordered_set>

// TODO(tnicolas42) docs -> class description
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

	void										play(float volume);
	void										pause();
	void										resume();
	void										stop();
	void										updateVolume(float volume, float previous_settings);
	bool										channelFinished(int chan);

private:
	Sound();
	Sound(const Sound &src);
	Sound										&operator=(const Sound &src);

	Mix_Chunk *									_chunk;
	std::unordered_set<int>						_currents_channels;
};

#endif
