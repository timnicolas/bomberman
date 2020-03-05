#include "Gui.hpp"
#include "Logging.hpp"
#include "Inputs.hpp"

// -- Gui ---------------------------------------------------------------
Gui::Gui(GameInfo &gameInfo)
: gameInfo(gameInfo),
  textureManager(nullptr),
  cubeShader(nullptr),
  cam(nullptr),
  _win(nullptr),
  _event(new SDL_Event()),
  _context(0),
  _skybox(nullptr) {}

Gui::~Gui() {
	logInfo("exit SDL");

	// free vao / vbo
	cubeShader->use();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &cubeShVbo);
	glDeleteVertexArrays(1, &cubeShVao);
	cubeShader->unuse();

	delete _event;
	delete textureManager;
	delete cubeShader;
	delete cam;
	delete _skybox;

	ABaseUI::destroy();

	// properly quit sdl
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_win);
    SDL_Quit();
}

Gui::Gui(Gui const &src)
: gameInfo(src.gameInfo) {
	*this = src;
}

Gui &Gui::operator=(Gui const &rhs) {
	if (this != &rhs) {
		logErr("The copy operator should not be called")
	}
	return *this;
}

void Gui::updateInput() {
	// manage inputs
	// quit
	if (Inputs::shouldQuit() || Inputs::getKey(InputType::Enum::CANCEL)) {
		logDebug("quiting...");
		gameInfo.quit = true;
	}
	// mouse motion
	cam->processMouseMovement(Inputs::getMouseRel().x, -Inputs::getMouseRel().y);

	float _dtTime = 0.01;  // TODO(zer0nim): need to get the correct dtTime

	// -- camera movement ------------------------------------------------------
	// camera movement
	if (Inputs::getKey(InputType::Enum::UP)) {
		cam->processKeyboard(CamMovement::Forward, _dtTime, false);
	}
	if (Inputs::getKey(InputType::Enum::RIGHT)) {
		cam->processKeyboard(CamMovement::Right, _dtTime, false);
	}
	if (Inputs::getKey(InputType::Enum::DOWN)) {
		cam->processKeyboard(CamMovement::Backward, _dtTime, false);
	}
	if (Inputs::getKey(InputType::Enum::LEFT)) {
		cam->processKeyboard(CamMovement::Left, _dtTime, false);
	}
}

// -- init ---------------------------------------------------------------------
bool	Gui::init() {
	logInfo("create gui");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		logErr("while loading SDL: " << SDL_GetError());
		SDL_Quit();
		return false;
	}

	// create window and init opengl settings
	if (!_initOpengl()) {
		return false;
	}

	// init shaders attributes
	if (!_initShaders()) {
		return false;
	}

	/* init UI interface */
	try {
		ABaseUI::init(s.j("font").s("file"), s.j("font").u("size"));
	}
	catch (ABaseUI::UIException & e) {
		logErr(e.what());
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
	_win = SDL_CreateWindow(gameInfo.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		gameInfo.windowSize.x, gameInfo.windowSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (_win == nullptr) {
		logErr("while loading OpenGL: " << SDL_GetError());
		return false;
	}

	// disable cursor for fps camera
	enableCursor(false);

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
	cubeShader = new Shader("shaders/cube_vs.glsl",
		"shaders/cube_fs.glsl",
		"shaders/cube_gs.glsl");
	cubeShader->use();

	// -- textureManager -------------------------------------------------------
	textureManager = new TextureManager();
	textureManager->setUniform(*cubeShader);

	// -- vao vbo --------------------------------------------------------------
	// create and bind vao and vbo
	glGenVertexArrays(1, &cubeShVao);
	glBindVertexArray(cubeShVao);
	glGenBuffers(1, &cubeShVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubeShVbo);

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
	cam = new Camera({0.0f, 25.0f, 0.0f});
	cam->lookAt(glm::vec3(gameInfo.gameboard[0] / 2 + 0.5f, 1.0f,
		gameInfo.gameboard[1] * 0.7f));

	float angle = cam->zoom;
	float ratio = static_cast<float>(gameInfo.windowSize.x) / gameInfo.windowSize.y;
	float nearD = 0.1f;
	float farD = 400;
	_projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

	// -- set default uniforms -------------------------------------------------
	// projection
	cubeShader->setMat4("projection", _projection);

	// cube material
	Material material;
	cubeShader->setVec3("material.specular", material.specular);
	cubeShader->setFloat("material.shininess", material.shininess);

	// direction light
	cubeShader->setVec3("dirLight.direction", -0.2f, -0.8f, 0.6f);
	cubeShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	cubeShader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	cubeShader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	// point light
	cubeShader->setFloat("pointLight.constant", 1.0f);
	cubeShader->setFloat("pointLight.linear", 0.09f);
	cubeShader->setFloat("pointLight.quadratic", 0.032f);

	cubeShader->setVec3("pointLight.ambient", 0.4f, 0.4f, 0.4f);
	cubeShader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	cubeShader->setVec3("pointLight.specular", 0.1f, 0.1f, 0.1f);
	// disabled for now
	cubeShader->setBool("pointLight.enabled", false);

	// disable texture transparency for now
	cubeShader->setBool("enableTransparency", true);

	// disable fog for now
	cubeShader->setBool("fog.enabled", false);

	cubeShader->unuse();

	// -- skybox ---------------------------------------------------------------
	_skybox = new Skybox;
	_skybox->getShader().use();
	_skybox->getShader().setMat4("projection", _projection);
	_skybox->getShader().unuse();

	return true;
}


// -- enableCursor -------------------------------------------------------------
void Gui::enableCursor(bool enable) {
	if (enable) {
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else {
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

// -- draw ---------------------------------------------------------------------
void Gui::preDraw() {
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gameInfo.windowSize.x, gameInfo.windowSize.y);
	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
}

void Gui::postDraw() {
	// swap buffer and check errors
	SDL_GL_SwapWindow(_win);
	checkError();
}

// -- _drawSkybox --------------------------------------------------------------
void	Gui::drawSkybox(glm::mat4 &view) {
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

// -- GameInfo struct ----------------------------------------------------------
GameInfo::GameInfo() {
	title = "bomberman";
	windowSize = {1200, 800};
	gameboard = {32, 32};
	player = {3, 3};
	play = State::S_PAUSE;
	quit = false;
}
