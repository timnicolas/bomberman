#ifndef AUDIOMANAGER_HPP
# define AUDIOMANAGER_HPP

# include <SDL2/SDL.h>
# include <SDL2/SDL_mixer.h>
# include <map>
# include "Sound.hpp"
# include "Music.hpp"

#define BOMB_EXPLOSION_SOUND "sounds/bomb_explosion.ogg"
#define PLAYER_HURT_SOUND "sounds/player_hurt.wav"
#define PLAYER_DEATH_SOUND "sounds/player_death.wav"
#define BONUS_SOUND "sounds/bonus.wav"
#define BONUS_DISAPPEAR_SOUND "sounds/bonus_disappear.wav"
#define NEW_LIFE_SOUND "sounds/new_life.wav"
#define END_OPEN_SOUND "sounds/door_open.wav"
#define FLAG_DESTROYED_SOUND "sounds/flag_destroyed.wav"
#define ENEMY_BASIC_DEATH_SOUND "sounds/enemy_basic_death.ogg"
#define ENEMY_FLY_DEATH_SOUND "sounds/enemy_fly_death.ogg"
#define ENEMY_FOLLOW_DEATH_SOUND "sounds/enemy_follow_death.wav"
#define ENEMY_WITH_EYE_DEATH_SOUND "sounds/enemy_with_eye_death.ogg"
#define ENEMY_FROG_DEATH_SOUND "sounds/enemy_frog_death.wav"
#define ENEMY_CRISPY_DEATH_SOUND "sounds/enemy_crispy_death.wav"

/**
 * @brief This is the audio manager. In this static class, you can play musics and sound
 */
class AudioManager {
public:
	static const int						nb_sound_channels = 42;

	~AudioManager();

	static AudioManager						&get();
	static bool								isEnabled();
	static void								updateSettings();

	static void								loadMusic(std::string file_name);
	static void								playMusic(std::string music_name, float volume = 1.0, bool loop = false);
	static void								pauseMusic();
	static void								resumeMusic();
	static void								stopMusic();
	static void								unloadMusic(std::string music_name);

	static void								loadSound(std::string file_name);
	static void								playSound(std::string sound_name, float volume = 1.0);
	static void								pauseSound(std::string sound_name);
	static void								resumeSound(std::string sound_name);
	static void								stopSound(std::string sound_name);
	static void								pauseAllSounds();
	static void								resumeAllSounds();
	static void								unloadSound(std::string sound_name);
	static void								stopAllSounds();

private:
	static std::string						_assets_path;

	AudioManager();
	AudioManager(const AudioManager &src);
	AudioManager							&operator=(const AudioManager &src);

	void									_updateSettings();

	void									_loadMusic(std::string file_name);
	void									_playMusic(std::string music_name, float volume = 1.0, bool loop = false);
	void									_pauseMusic();
	void									_resumeMusic();
	void									_stopMusic();
	void									_unloadMusic(std::string music_name);

	void									_loadSound(std::string file_name);
	void									_playSound(std::string sound_name, float volume);
	void									_pauseSound(std::string sound_name);
	void									_resumeSound(std::string sound_name);
	void									_stopSound(std::string sound_name);
	void									_pauseAllSounds();
	void									_resumeAllSounds();
	void									_unloadSound(std::string sound_name);
	void									_stopAllSounds();

	static void								_channelHalted(int chan);

	float									_volume_master;
	float									_volume_music;
	float									_volume_sound;
	float									_music_modifier;
	std::map<std::string, Music *>			_musics;
	std::map<std::string, Sound *>			_sounds;
	bool									_enabled;
};

#endif
