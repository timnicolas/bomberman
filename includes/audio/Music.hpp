#ifndef MUSIC_HPP
# define MUSIC_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <string>

class Music {
public:
	explicit Music(std::string filename);
	~Music();

	void									play(float volume, bool loop);
private:
	Music();
	Music(const Music &src);
	Music									&operator=(const Music &src);

	std::exception							_filename;
	Mix_Music								*_mix_mus;
};

#endif
