#ifndef AUDIOMANAGER_HPP
# define AUDIOMANAGER_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <map>

class AudioManager {
public:
	static const int						nbSoundChannels = 10;
	~AudioManager();

	static AudioManager						&get();
	static void								loadMusic(std::string filename);
	static void								loadSound(std::string filename);
	static void								playMusic();
	static void								pauseMusic();
	static void								playSound();
	static void								stopSound();
	static void								stopAllSounds();

private:
	AudioManager();
	AudioManager(const AudioManager &src);
	AudioManager							&operator=(const AudioManager &src);

	void									_loadMusic(std::string filename);
	void									_loadSound(std::string filename);
	void									_playMusic();
	void									_pauseMusic();
	void									_playSound();
	void									_stopSound();
	void									_stopAllSounds();

	float									_volumeMusic;
	float									_volumeSound;
	std::map<std::string, Mix_Music *>		_musics;
	std::map<std::string, Mix_Chunk *>		_sounds;
	bool									_enabled;
};

#endif
