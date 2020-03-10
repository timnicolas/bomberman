#pragma once

#define C_VAO_WIDTH 4
#define C_NB_FACES 6
// C_VAO_WIDTH * C_NB_FACES
#define C_FACE_A_SIZE 24
#define CAM_POS_OFFSET glm::vec3(0.0f, 20.0f, -10.0f)
#define CAM_TARG_OFFSET glm::vec3(0.0f, 1.0f, 0.0f)
#define CAM_SPEED 1.5f
#define VOID_POS glm::ivec2 {-1, -1}
#define VOID_POS3 glm::vec3 {-1, -1, -1}

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
#include "debug.hpp"
#include "ABaseUI.hpp"

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

		void	updateInput(float const dtTime);
		bool	init();
		void	preDraw();
		void	postDraw();
		void	drawSkybox(glm::mat4 &view);
		void	enableCursor(bool enable);
		void	drawCube(Block::Enum typeBlock, glm::vec3 pos = {0, 0, 0}, glm::vec3 scale = {1, 1, 1});

		GameInfo		&gameInfo;
		TextureManager	*textureManager;
		Shader			*cubeShader;
		Camera			*cam;

		uint32_t		cubeShVao;
		uint32_t		cubeShVbo;

	private:
		SDL_Window		*_win;
		SDL_Event		*_event;
		SDL_GLContext	_context;

		Skybox			*_skybox;

		glm::mat4		_projection;

		static std::array<float, C_FACE_A_SIZE> const		_cubeFaces;

		Gui();  // private, should not be called
		bool	_init();
		bool	_initOpengl();
		bool	_initShaders();
};
