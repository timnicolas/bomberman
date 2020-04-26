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
#define PLAYER_RUN_SOUND "sounds/player_run.ogg"
#define PUT_BOMB_SOUND "sounds/put_bomb.ogg"
#define PUT_BOMB_EMPTY_SOUND "sounds/put_no_bomb.ogg"
#define PLAYER_ONE_LIFE_SOUND "sounds/one_life.ogg"
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
#define ENEMY_HIT_1_SOUND "sounds/enemy_hit_1.ogg"
#define ENEMY_HIT_2_SOUND "sounds/enemy_hit_2.ogg"
#define ENEMY_SHOT_SOUND "sounds/enemy_shot.ogg"
#define WIN_SOUND "sounds/win_music.ogg"
#define GAME_OVER_SOUND "sounds/game_over.ogg"
#define MONSTERKILL_SOUND "sounds/monster_kill.ogg"
#define INTROLEVEL_SOUND "sounds/intro_level.ogg"
#define ALARM_TIME_SOUND "sounds/alarm_time.ogg"

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
	static void								playSound(std::string sound_name, float volume = 1.0, bool loop = false);
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
	void									_playSound(std::string sound_name, float volume = 1.0, bool loop = false);
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
