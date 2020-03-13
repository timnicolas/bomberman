#ifndef MUSIC_HPP
# define MUSIC_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <string>
#include <stdexcept>

/**
 * @brief Music class for AutioManager
 */
class Music {
public:
	class MusicException: public std::runtime_error {
		public:
			MusicException();
			explicit MusicException(const char* what_arg);
			explicit MusicException(const std::string what_arg);
	};

	explicit Music(std::string filename);
	~Music();

	void									play(float volume, bool loop);

private:
	Music();
	Music(const Music &src);
	Music									&operator=(const Music &src);

	Mix_Music								*_mix_mus;
};

#endif
