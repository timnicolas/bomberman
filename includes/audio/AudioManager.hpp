#ifndef AUDIOMANAGER_HPP
# define AUDIOMANAGER_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <map>
# include "Sound.hpp"

class AudioManager {
public:
	static const int						nb_sound_channels = 42;
	~AudioManager();

	static AudioManager						&get();
	static bool								isEnabled();
	static void								updateSettings();
	static void								loadMusic(std::string filename);
	static void								loadSound(std::string filename);
	static void								playMusic();
	static void								pauseMusic();
	static void								playSound(std::string sound_name, float volume);
	static void								unloadSound(std::string sound_name);
	static void								stopAllSounds();

private:
	AudioManager();
	AudioManager(const AudioManager &src);
	AudioManager							&operator=(const AudioManager &src);

	void									_updateSettings();
	void									_loadMusic(std::string filename);
	void									_loadSound(std::string filename);
	void									_playMusic();
	void									_pauseMusic();
	void									_playSound(std::string sound_name, float volume);
	void									_unloadSound(std::string sound_name);
	void									_stopAllSounds();

	float									_volume_master;
	float									_volume_music;
	float									_volume_sound;
	std::map<std::string, Mix_Music *>		_musics;
	std::map<std::string, Sound *>			_sounds;
	bool									_enabled;
};

#endif
