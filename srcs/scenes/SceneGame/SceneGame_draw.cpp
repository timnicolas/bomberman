#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "SceneGame.hpp"
#include "bomberman.hpp"
#include "FileUtils.hpp"
#include "Save.hpp"
#include "ModelsManager.hpp"
#include "BoxCollider.hpp"

#include "Player.hpp"
#include "Wall.hpp"
#include "Crispy.hpp"
#include "Flag.hpp"
#include "End.hpp"

#include "EnemyBasic.hpp"
#include "EnemyFollow.hpp"
#include "EnemyWithEye.hpp"
#include "EnemyFly.hpp"
#include "EnemyCrispy.hpp"
#include "EnemyFrog.hpp"

#include "AudioManager.hpp"
#include "SceneManager.hpp"

/**
 * @brief draw is called each frame to draw the Game Scene.
 *
 * @return true
 * @return false
 */
bool	SceneGame::draw() {
	if (level == NO_LEVEL)
		return drawForMenu();
	return drawGame();
}

/**
 * @brief Draw function if we are in a menu (no level loaded)
 *
 * @return false If failed
 */
bool	SceneGame::drawForMenu() {
	blurFilterBefore();  // blur filter settings, enable framebuffer

	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-0.9, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|idle")
			_menuModels.player->setAnimation("Armature|idle");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({0.9, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|idle")
			_menuModels.flower->setAnimation("Armature|idle");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	blurFilterAfter();  // blur filter postprocess

	return true;
}

/**
 * @brief Draw function if we are in a level
 *
 * @return false If failed
 */
bool	SceneGame::drawGame() {
	if (state != GameState::INTRO) {
		blurFilterBefore();  // blur filter settings, enable framebuffer
	}

	// draw background terrain
	if (s.j("debug").j("show").b("terrain")) {
		try {
			_terrain->draw();
		}
		catch(OpenGLModel::ModelException const & e) {
			logErr(e.what());
			return false;
		}
	}

	// draw floor
	if (s.j("debug").j("show").b("baseBoard")) {
		for (uint32_t j = 0; j < size.y; j++) {
			for (uint32_t i = 0; i < size.x; i++) {
				if (floor[i][j])
					_gui->drawCube(Block::FLOOR, {i, -1.5f, j}, {1, 1.5f, 1});
			}
		}
	}

	std::vector<AEntity *>  endBlocs;

	// draw board entities
	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (board_it2->type == Type::END) {
					endBlocs.push_back(board_it2);
				}
				else {
					if (s.j("debug").j("show").b("baseBoard") && !board_it2->draw(*_gui))
						return false;
					if (s.j("debug").j("show").b("staticCollider") && !board_it2->drawCollider())
						return false;
				}
			}
		}
	}

	// draw flyng obstacles
	for (auto &&board_it0 : boardFly) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (s.j("debug").j("show").b("flyHeight") && !board_it2->draw(*_gui))
					return false;
				if (s.j("debug").j("show").b("staticCollider") && !board_it2->drawCollider())
					return false;
			}
		}
	}

	// draw enemies
	for (auto &&enemy : enemies) {
		if (s.j("debug").j("show").b("entity") && !enemy->draw(*_gui))
			return false;
		if (s.j("debug").j("show").b("movingCollider") && !enemy->drawCollider())
			return false;
	}

	// draw player
	if (s.j("debug").j("show").b("entity") && !player->draw(*_gui))
		return false;
	if (s.j("debug").j("show").b("movingCollider") && !player->drawCollider())
		return false;

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	// draw spawners
	for (auto &&spawner : spawners) {
		if (!spawner->draw(*_gui))
			return false;
	}

	// draw end blocks (last for transparency issue)
	for (AEntity *end : endBlocs) {
		if (s.j("debug").j("show").b("baseBoard") && !end->draw(*_gui))
			return false;
		if (s.j("debug").j("show").b("staticCollider") && !end->drawCollider())
			return false;
	}
	endBlocs.clear();

	// release cubeShader and textures
	_gui->cubeShader->use();
	_gui->textureManager->disableTextures();
	_gui->cubeShader->unuse();

	// draw ui on top
	if (state == GameState::PLAY
		&& (allUI.timeLeftImg->getPos() != VOID_SIZE || allUI.timeLeftImgActive->getPos() != VOID_SIZE))
	{
		ASceneMenu::draw();
	}
	else if (state == GameState::INTRO) {
		allUI.introText->draw();
	}

	if (state != GameState::INTRO) {
		blurFilterAfter();  // blur filter postprocess
	}

	return true;
}

/**
 * @brief Draw function if we are in victory menu
 *
 * @return false If failed
 */
bool	SceneGame::drawVictory() {
	blurFilterBefore();  // blur filter settings, enable framebuffer

	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-1, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|dance")
			_menuModels.player->setAnimation("Armature|dance");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({1, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|loose")
			_menuModels.flower->setAnimation("Armature|loose");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	blurFilterAfter();  // blur filter postprocess

	return true;
}

/**
 * @brief Draw function if we are in game over menu
 *
 * @return false If failed
 */
bool	SceneGame::drawGameOver() {
	blurFilterBefore();  // blur filter settings, enable framebuffer

	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-1, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|loose")
			_menuModels.player->setAnimation("Armature|loose");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({1, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|dance")
			_menuModels.flower->setAnimation("Armature|dance");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	blurFilterAfter();  // blur filter postprocess

	return true;
}

/**
 * @brief Draw function if we are in endGame menu
 *
 * @return false If failed
 */
bool	SceneGame::drawEndGame() {
	/* draw simple floor */
	_gui->drawCube(Block::FLOOR, {-10.0, -1.5, -5.0}, {20.0, 0.5, 5.0});

	/* draw models */
	try {
		float tmpX = _menuModels.player->transform.getPos().x;
		if (tmpX < -3 || tmpX > 10)
			tmpX = -3;
		tmpX += 0.03;

		_menuModels.player->animationSpeed = 0.8;
		_menuModels.player->transform.setPos({tmpX, -1, -2});
		_menuModels.player->transform.setRot(glm::radians(90.0));
		if (_menuModels.player->getCurrentAnimationName() != "Armature|run")
			_menuModels.player->setAnimation("Armature|run");
		_menuModels.player->draw();

		tmpX -= 1.3;
		_menuModels.robot->transform.setPos({tmpX, -1, -2});
		_menuModels.robot->transform.setRot(glm::radians(90.0));
		if (_menuModels.robot->getCurrentAnimationName() != "Armature|run")
			_menuModels.robot->setAnimation("Armature|run");
		_menuModels.robot->draw();

		tmpX -= 1.3;
		_menuModels.flower->transform.setPos({tmpX, -1, -2});
		_menuModels.flower->transform.setRot(glm::radians(90.0));
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|run")
			_menuModels.flower->setAnimation("Armature|run");
		_menuModels.flower->draw();

		tmpX -= 1.3;
		_menuModels.crispy->transform.setPos({tmpX, -1, -2});
		_menuModels.crispy->transform.setRot(glm::radians(90.0));
		if (_menuModels.crispy->getCurrentAnimationName() != "Armature|run")
			_menuModels.crispy->setAnimation("Armature|run");
		_menuModels.crispy->draw();

		tmpX -= 1.3;
		_menuModels.follow->transform.setPos({tmpX, -1, -2});
		_menuModels.follow->transform.setRot(glm::radians(90.0));
		if (_menuModels.follow->getCurrentAnimationName() != "Armature|run")
			_menuModels.follow->setAnimation("Armature|run");
		_menuModels.follow->draw();

		tmpX -= 1.3;
		_menuModels.fly->transform.setPos({tmpX, .2, -2});
		_menuModels.fly->transform.setRot(glm::radians(90.0));
		if (_menuModels.fly->getCurrentAnimationName() != "Armature|run")
			_menuModels.fly->setAnimation("Armature|run");
		_menuModels.fly->draw();

		_menuModels.frog->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief get intro animation
 *
 * @return std::vector<CamPoint> Animation
 */
std::vector<CamPoint>	SceneGame::_getIntroAnim() const {
	return {
		{
			{80, 80, 150},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{size.x / 2, 32, size.y / 2},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
		{
			{size.x / 2 + 5, 45, size.y / 2 + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 3,  // speed
		},

		{
			{-15, 20, -15},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{-15, 20, size.y + 15},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			-1,  // speed
		},

		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			-1,  // speed
		},
	};
}

/**
 * @brief get game over animation
 *
 * @return std::vector<CamPoint> Animation
 */
std::vector<CamPoint>	SceneGame::_getGameOverAnim() const {
	return {
		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 7, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 20, player->position.z + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 5,  // speed
		},
		{
			{player->position.x, player->position.y + 1, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
	};
}

/**
 * @brief get victory animation
 *
 * @return std::vector<CamPoint> Animation
 */
std::vector<CamPoint>	SceneGame::_getVictoryAnim() const {
	return {
		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 7, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 20, player->position.z + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 5,  // speed
		},
		{
			{player->position.x, player->position.y + 1, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
	};
}

/**
 * @brief Call this function before drawing operation you want the ui to blur
 */
void	SceneGame::blurFilterBefore() {
	// first pass, draw the scene in framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _blurFbo[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}
/**
 * @brief Call this function after drawing operation you want the ui to blur
 */
void	SceneGame::blurFilterAfter() {
	// Second pass the apply blur effect
	uint32_t	nbPass = 6;
	uint32_t	totalPass = nbPass * 2;
	bool		horizontal = true;

	glDisable(GL_DEPTH_TEST);
	for (uint32_t i = 0; i < totalPass; ++i) {
		if (i < totalPass - 1) {
			glBindFramebuffer(GL_FRAMEBUFFER, _blurFbo[(i + 1) % 2]);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		glClear(GL_COLOR_BUFFER_BIT);

		_blurShader->use();
		_blurShader->setBool("horizontal", horizontal);
		glBindVertexArray(_ppShVao);
		// fbo texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _blurTexColor[i % 2]);
		// mask texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _blurMaskTex);
		// draw the quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glActiveTexture(GL_TEXTURE0);
		_blurShader->unuse();

		horizontal = !horizontal;
	}
	glEnable(GL_DEPTH_TEST);
}
