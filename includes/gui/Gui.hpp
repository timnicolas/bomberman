#pragma once

#define C_VAO_WIDTH 4
#define C_NB_FACES 6
// C_VAO_WIDTH * C_NB_FACES
#define C_FACE_A_SIZE 42
#define CAM_POS_OFFSET glm::vec3(0.0f, 20.0f, -10.0f)
#define CAM_TARG_OFFSET glm::vec3(0.0f, 1.0f, 0.0f)
#define CAM_SPEED 1.5f
#define VOID_POS glm::ivec2 {-1, -1}

#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <map>

#include "bomberman.hpp"
#include "Logging.hpp"
#include "includesOpengl.hpp"
#include "TextureManager.hpp"
#include "TextRender.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "UI/Button.hpp"
#include "debug.hpp"

#define TITLE	"bomberman"

// TODO(zer0nim): move GameInfo
namespace State {
	enum Enum {
		S_PLAY,
		S_PAUSE,
		S_GAMEOVER,
	};
}

struct GameInfo {
	std::string	title;
	glm::ivec2	windowSize;
	glm::ivec2	gameboard;
	glm::ivec2	player;
	State::Enum	play;
	bool		quit;

	GameInfo();
};

class Gui {
	public:
		explicit	Gui(GameInfo &gameInfo);
		virtual ~Gui();
		Gui(Gui const &src);
		Gui &operator=(Gui const &rhs);

		void	updateInput();
		bool	init();
		bool	draw();

	private:
		GameInfo		&_gameInfo;
		SDL_Window		*_win;
		SDL_Event		*_event;
		SDL_GLContext	_context;

		TextureManager	*_textureManager;
		Shader			*_cubeShader;
		Camera			*_cam;
		TextRender		*_textRender;
		Skybox			*_skybox;
		Button			*_button;

		uint32_t		_cubeShVao;
		uint32_t		_cubeShVbo;
		glm::mat4		_projection;

		static std::array<float, C_FACE_A_SIZE> const		_cubeFaces;

		Gui();  // private, should not be called
		bool	_init();
		bool	_initOpengl();
		bool	_initShaders();
		void	_drawBoard();
		void	_drawSkybox(glm::mat4 &view);
};
