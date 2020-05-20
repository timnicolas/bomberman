#include "BoxCollider.hpp"
#include "Logging.hpp"

const float	BoxCollider::_boxVertices[] = {
	0, 0, 0,
	0, 0, 1,

	1, 0, 0,
	1, 0, 1,

	0, 0, 0,
	1, 0, 0,

	0, 0, 1,
	1, 0, 1,

	0, 1, 0,
	0, 1, 1,

	1, 1, 0,
	1, 1, 1,

	0, 1, 0,
	1, 1, 0,

	0, 1, 1,
	1, 1, 1,

	0, 0, 0,
	0, 1, 0,

	0, 0, 1,
	0, 1, 1,

	1, 0, 0,
	1, 1, 0,

	1, 0, 1,
	1, 1, 1,
};

/**
 * @brief Construct a new Box Collider:: Box Collider object
 */
BoxCollider::BoxCollider()
: _gui(nullptr),
  _boxShader(nullptr) {}

/**
 * @brief Construct a new Box Collider:: Box Collider object
 *
 * @param src The object to do the copy
 */
BoxCollider::BoxCollider(BoxCollider const & src) {
	*this = src;
}

/**
 * @brief Destroy the Box Collider:: Box Collider object
 */
BoxCollider::~BoxCollider() {
}

/**
 * @brief Init the BoxCollider object (call only once)
 *
 * @return false If failed
 */
bool BoxCollider::init(Gui * gui) {
	return BoxCollider::get()._init(gui);
}
/**
 * @brief Init the BoxCollider object (call only once)
 *
 * @return false If failed
 */
bool BoxCollider::_init(Gui * gui) {
	_gui = gui;
	_boxShader = new Shader(BOX_SHADER_VS, BOX_SHADER_FS);
	_boxShader->use();
	_boxShader->setMat4("projection", _gui->cam->getProjection());
    glGenVertexArrays(1, &_boxShaderVAO);
    glGenBuffers(1, &_boxShaderVBO);

    glBindVertexArray(_boxShaderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _boxShaderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxCollider::_boxVertices), BoxCollider::_boxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	_boxShader->unuse();
	return true;
}

/**
 * @brief Destroy the BoxCollider object (call only once)
 *
 * @return false If failed
 */
bool BoxCollider::destroy() {
	return BoxCollider::get()._destroy();
}
/**
 * @brief Destroy the BoxCollider object (call only once)
 *
 * @return false If failed
 */
bool BoxCollider::_destroy() {
	if (_boxShader == nullptr) {
		logErr("you need to init BoxCollider before destroy it");
		return false;
	}
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_boxShaderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &_boxShaderVBO);
	delete _boxShader;
	_boxShader = nullptr;
	return true;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return BoxCollider& A reference to the copied object
 */
BoxCollider & BoxCollider::operator=(BoxCollider const & rhs) {
	if (this != &rhs) {
		logWarn("BoxCollider object copied");
	}
	return *this;
}

/**
 * @brief Get the instance of BoxCollider object
 *
 * @return BoxCollider& The instance
 */
BoxCollider &	BoxCollider::get() {
	static BoxCollider	instance;
	return instance;
}

/**
 * @brief Draw a box collider on the screen
 *
 * @param pos The position
 * @param size The size
 * @param color The color
 */
bool BoxCollider::drawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color) {
	return BoxCollider::get()._drawBox(pos, size, color);
}
/**
 * @brief Draw a box collider on the screen
 *
 * @param pos The position
 * @param size The size
 * @param color The color
 */
bool BoxCollider::_drawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color) {
	if (_boxShader == nullptr) {
		logErr("You need to init BoxCollider before draw (BoxCollider::init)");
		return false;
	}
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, pos);
	model = glm::scale(model, size);
	_boxShader->use();
	_boxShader->setMat4("view", _gui->cam->getViewMatrix());
	_boxShader->setMat4("model", model);
	_boxShader->setVec4("color", color);
	glBindVertexArray(_boxShaderVAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
	_boxShader->unuse();
	return true;
}
