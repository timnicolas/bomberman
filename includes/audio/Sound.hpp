#ifndef SOUND_HPP
# define SOUND_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <string>
# include <unordered_set>

class Sound {
public:
	explicit Sound(std::string filename);
	~Sound();

	void										play(float volume);
	void										stop();
	void										updateVolume(float volume, float previous_settings);
	bool										channelFinished(int chan);
private:
	Sound();
	Sound(const Sound &src);
	Sound										&operator=(const Sound &src);

	std::string									_filename;
	Mix_Chunk *									_chunk;
	std::unordered_set<int>						_currents_channels;
};

#endif
