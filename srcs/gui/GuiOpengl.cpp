#include "Gui.hpp"

#include "Material.hpp"
#include "debug.hpp"

// -- init ---------------------------------------------------------------------
bool Gui::init(GameInfo &gameInfo) {
	this->gameInfo = &gameInfo;

	logInfo("loading SDL");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		logErr("while loading SDL: " << SDL_GetError());
		SDL_Quit();
		return false;
	}

	// create window and init opengl settings
	if (!_initOpengl()) {
		return false;
	}

	// create text manager
	_textManager = new TextManager(this->gameInfo);
	if (!_textManager->init()) {
		return false;
	}

	// init shaders attributes
	if (!_initShaders()) {
		return false;
	}

	return true;
}

// -- _initOpengl --------------------------------------------------------------
bool	Gui::_initOpengl() {
	// opengl version 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	_win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		gameInfo->windowSize.x, gameInfo->windowSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (_win == nullptr) {
		logErr("while loading OpenGL: " << SDL_GetError());
		return false;
	}

	// disable cursor for fps camera
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// create opengl context
	_context = SDL_GL_CreateContext(_win);
	if (_context == 0) {
		logErr("while loading OpenGL: " << SDL_GetError());
		return false;
	}
	// init glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		logErr("while loading OpenGL: failed to init glad");
		return false;
	}

	glEnable(GL_MULTISAMPLE);  // anti aliasing
	glEnable(GL_CULL_FACE);  // face culling
	glEnable(GL_BLEND);  // enable blending (used in textRender)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	return true;
}

// -- _initShaders -------------------------------------------------------------
// create opengl shader stuffs here (buffers, camera, ...)
bool	Gui::_initShaders() {
	// -- create shader -`------------------------------------------------------
	_cubeShader = new Shader("shaders/cube_vs.glsl",
		"shaders/cube_fs.glsl",
		"shaders/cube_gs.glsl");
	_cubeShader->use();

	// -- textureManager -------------------------------------------------------
	_textureManager = new TextureManager();
	_textureManager->setUniform(*_cubeShader);

	// -- vao vbo --------------------------------------------------------------
	// create and bind vao and vbo
	glGenVertexArrays(1, &_cubeShVao);
	glBindVertexArray(_cubeShVao);
	glGenBuffers(1, &_cubeShVbo);
	glBindBuffer(GL_ARRAY_BUFFER, _cubeShVbo);

	// fill buffer
	glBufferData(GL_ARRAY_BUFFER, _cubeFaces.size() * sizeof(float),
		&_cubeFaces[0], GL_STATIC_DRAW);

	// bottom left corner face pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, C_VAO_WIDTH * sizeof(float),
		reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	// face id
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, C_VAO_WIDTH * sizeof(float),
		reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind vao / vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// -- camera ---------------------------------------------------------------
	_cam = new Camera({
		0.0f,
		25.0f,
		0.0f});

	float angle = _cam->zoom;
	float ratio = static_cast<float>(gameInfo->windowSize.x) / gameInfo->windowSize.y;
	float nearD = 0.1f;
	float farD = 400;
	_projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

	// -- set default uniforms -------------------------------------------------
	// projection
	_cubeShader->setMat4("projection", _projection);

	// cube material
	Material material;
	_cubeShader->setVec3("material.specular", material.specular);
	_cubeShader->setFloat("material.shininess", material.shininess);

	// direction light
	_cubeShader->setVec3("dirLight.direction", -0.2f, -0.8f, 0.6f);
	_cubeShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_cubeShader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	_cubeShader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	// point light
	_cubeShader->setFloat("pointLight.constant", 1.0f);
	_cubeShader->setFloat("pointLight.linear", 0.09f);
	_cubeShader->setFloat("pointLight.quadratic", 0.032f);

	_cubeShader->setVec3("pointLight.ambient", 0.4f, 0.4f, 0.4f);
	_cubeShader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	_cubeShader->setVec3("pointLight.specular", 0.1f, 0.1f, 0.1f);
	// disabled for now
	_cubeShader->setBool("pointLight.enabled", false);

	// disable texture transparency for now
	_cubeShader->setBool("enableTransparency", true);

	// disable fog for now
	_cubeShader->setBool("fog.enabled", false);

	_cubeShader->unuse();

	// -- skybox ---------------------------------------------------------------
	_skybox = new Skybox;
	_skybox->getShader().use();
	_skybox->getShader().setMat4("projection", _projection);
	_skybox->getShader().unuse();

	// -------------------------------------------------------------------------
	_lastLoopMs = _getMs().count();  // init first frame

	return true;
}


// -- draw ---------------------------------------------------------------------
bool Gui::draw() {
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gameInfo->windowSize.x, gameInfo->windowSize.y);
	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);

	// use cubeShader, set uniform and activate textures
	glm::mat4	view = _cam->getViewMatrix();
	_cubeShader->use();
	_cubeShader->setMat4("view", view);
	_cubeShader->setVec3("viewPos", _cam->pos);
	glBindVertexArray(_cubeShVao);
	_textureManager->activateTextures();
	_cubeShader->setInt("blockId", 0);

	// move camera to follow snake head
	if (gameInfo->snake.size() > 0) {
		glm::ivec2	head = gameInfo->snake.front();
		if (_camDirAngle.find(gameInfo->direction) != _camDirAngle.end()) {
			glm::vec4	posOffset = glm::vec4(CAM_POS_OFFSET, 0.0f);
			posOffset.z = -posOffset.z;  // invert z
			glm::vec4	target = glm::vec4(CAM_TARG_OFFSET, 0.0f);
			target.z = -target.z;  // invert z

			// calculate desired pos
			glm::vec3	desiredPos = glm::vec3(head.x, 0.0f, head.y)
				+ glm::vec3(posOffset);

			// apply values
			_cam->pos = desiredPos;
			_cam->lookAt(glm::vec3(head.x, 0.0f, head.y) + glm::vec3(target));
		}
	}

	// draw scene
	_drawBoard();
	_drawSnake();
	_drawFood();

	// release cubeShader and textures
	_textureManager->disableTextures();
	_cubeShader->unuse();

	// draw skybox
	_drawSkybox(view);

	// draw text
	_textManager->draw();

	// swap buffer and check errors
	SDL_GL_SwapWindow(_win);
	checkError();
	return true;
}

// -- _drawBoard ---------------------------------------------------------------
void	Gui::_drawBoard() {
	glm::mat4 model(1.0);
	glm::vec3 pos;

	// board floor
	_cubeShader->setVec3("blockSize",
		{gameInfo->gameboard.x, 1.0f, gameInfo->gameboard.y});
	_cubeShader->setInt("blockId", Block::B_FLOOR);  // set block type
	pos = glm::vec3(0.0f, -1.0f, gameInfo->gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 0
	_cubeShader->setVec3("blockSize",
		{gameInfo->gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(0.0f, -1.0f, gameInfo->gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 1
	_cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, gameInfo->gameboard.y + 1.0f});
	pos = glm::vec3(gameInfo->gameboard.x, -1.0f, gameInfo->gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 2
	_cubeShader->setVec3("blockSize",
		{gameInfo->gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, -1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 3
	_cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, gameInfo->gameboard.y + 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, gameInfo->gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// water
	_cubeShader->setVec3("blockSize",
		{gameInfo->gameboard.x, 1.0f, gameInfo->gameboard.y});
	_cubeShader->setInt("blockId", Block::WATER);  // set block type
	pos = glm::vec3(0.0f, 0.0f, gameInfo->gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
}

// -- _drawSnake ---------------------------------------------------------------
void	Gui::_drawSnake() {
	glm::mat4 model(1.0);
	glm::vec3 pos;

	_cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	for (auto it = gameInfo->snake.begin(); it != gameInfo->snake.end(); ++it) {
		// set block type
		_cubeShader->setInt("blockId", (
			it == gameInfo->snake.begin()
			? Block::SNAKE_HEAD : Block::SNAKE_BODY));
		// set block pos
		pos = glm::vec3((*it)[0], 0.5f, (*it)[1]);
		model = glm::translate(glm::mat4(1.0), pos);
		_cubeShader->setMat4("model", model);
		glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
	}
}
// -- _drawFood ----------------------------------------------------------------
void	Gui::_drawFood() {
	glm::mat4 model(1.0);
	glm::vec3 pos;

	if (gameInfo->food != VOID_POS) {
		// set block type
		_cubeShader->setInt("blockId", Block::FOOD);
		// set block pos
		pos = glm::vec3(gameInfo->food[0], 0.5f, gameInfo->food[1]);
		model = glm::translate(glm::mat4(1.0), pos);
		_cubeShader->setMat4("model", model);
		glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
	}
}

// -- _drawSkybox ----------------------------------------------------------------
void	Gui::_drawSkybox(glm::mat4 &view) {
	CAMERA_MAT4	skyView = view;
	skyView[3][0] = 0;  // remove translation for the skybox
	skyView[3][1] = 0;
	skyView[3][2] = 0;
	_skybox->getShader().use();
	_skybox->getShader().setMat4("view", skyView);
	_skybox->getShader().unuse();
	_skybox->draw(0.5);
	_skybox->getShader().unuse();
}

// -- statics const ------------------------------------------------------------
// cube faces
std::array<float, C_FACE_A_SIZE> const	Gui::_cubeFaces = {
	// bot left corner,		faceId
	-0.5f, -0.5f, 0.5f,		0,
	0.5f, -0.5f, 0.5f,		1,
	0.5f, -0.5f, -0.5f,		2,
	-0.5f, -0.5f, -0.5f,	3,
	-0.5f, 0.5f, 0.5f,		4,
	-0.5f, -0.5f, -0.5f,	5,
};

// cam position according to snake direction
std::unordered_map<Direction::Enum, float, EnumClassHash> const	Gui::_camDirAngle = {
	{Direction::MOVE_UP, 0.0f},  // 0°
	{Direction::MOVE_RIGHT, 1.5708f},  // 90°
	{Direction::MOVE_DOWN, 3.14159f},  // 180°
	{Direction::MOVE_LEFT, 4.71239f}  // 270°
};
