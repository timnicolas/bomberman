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
 * init game method.
 */
bool			SceneGame::init() {
	int32_t i = 0;
	while (_initJsonLevel(i)) {
		if (i >= 100000) {  // max level
			break;
		}
		i++;
	}

	try {
		/* load all models for menu */
		_menuModels.player = new Model(ModelsManager::getModel("white"), getDtTime(),
			ETransform({0, 0, 0}, PLAYER_SIZE));
		_menuModels.player->play = true;
		_menuModels.player->loopAnimation = true;
		_menuModels.player->setAnimation("Armature|idle");

		_menuModels.flower = new Model(ModelsManager::getModel("flower"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_WITH_EYE_SIZE));
		_menuModels.flower->play = true;
		_menuModels.flower->loopAnimation = true;
		_menuModels.flower->setAnimation("Armature|idle");

		_menuModels.robot = new Model(ModelsManager::getModel("robot"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_BASIC_SIZE));
		_menuModels.robot->play = true;
		_menuModels.robot->animationSpeed = 1.6;
		_menuModels.robot->loopAnimation = true;
		_menuModels.robot->setAnimation("Armature|idle");

		_menuModels.fly = new Model(ModelsManager::getModel("flyingBot"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_FLY_SIZE));
		_menuModels.fly->play = true;
		_menuModels.fly->loopAnimation = true;
		_menuModels.fly->setAnimation("Armature|run");

		_menuModels.frog = new Model(ModelsManager::getModel("frog"), getDtTime(),
		ETransform({0, 0, 0}, ENEMY_FROG_SIZE));
		_menuModels.frog->play = true;
		_menuModels.frog->loopAnimation = true;
		_menuModels.frog->setAnimation("Armature|idle");
		_menuModels.frog->transform.setPos({-2, -1, -4});
		_menuModels.frog->transform.setRot(glm::radians(30.0));

		_menuModels.crispy = new Model(ModelsManager::getModel("crispyEnemy"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_CRISPY_SIZE));
		_menuModels.crispy->play = true;
		_menuModels.crispy->animationSpeed = 1.6;
		_menuModels.crispy->loopAnimation = true;
		_menuModels.crispy->setAnimation("Armature|run");

		_menuModels.follow = new Model(ModelsManager::getModel("follow"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_FOLLOW_SIZE));
		_menuModels.follow->play = true;
		_menuModels.follow->animationSpeed = 1.4;
		_menuModels.follow->loopAnimation = true;
		_menuModels.follow->setAnimation("Armature|idle");

		// init terrain model
		if (!_terrain) {
			_terrain = new Model(ModelsManager::getModel("terrain"), getDtTime(),
			ETransform({0, -2, 0}));
		}

		/* loading sentence */
		allUI.introText = &addText({0, 0}, {_gui->gameInfo.windowSize.x, 70}, "")
			.setTextAlign(TextAlign::CENTER)
			.setColor(colorise(s.j("colors").j("bg-rect").u("color"), s.j("colors").j("bg-rect").u("alpha")))
			.setZ(0.5);
	}
	catch(ModelsManager::ModelsManagerException const &e) {
		logErr(e.what());
		return false;
	}
	catch(OpenGLModel::ModelException const &e) {
		logErr(e.what());
		return false;
	}

	_initGameInfos();

	return true;
}

/**
 * @brief Init game informations
 */
void SceneGame::_initGameInfos() {
	try {
		allUI.timeLeftImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/time.png");
		allUI.timeLeftImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/time_active.png");
		allUI.timeLeftText = &addText(VOID_SIZE, VOID_SIZE, "time-left").setTextAlign(TextAlign::RIGHT);
		allUI.scoreImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/score.png");
		allUI.scoreImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/score_active.png");
		allUI.scoreText = &addText(VOID_SIZE, VOID_SIZE, "score").setTextAlign(TextAlign::RIGHT);
		allUI.lifeImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/life.png");
		allUI.lifeImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/life_active.png");
		allUI.lifeText = &addText(VOID_SIZE, VOID_SIZE, "nb-player-lives").setTextAlign(TextAlign::RIGHT);
		allUI.levelNameText = &addText(VOID_SIZE, VOID_SIZE, "level-name").setTextAlign(TextAlign::RIGHT);
		allUI.enemiesCounterText = &addText(VOID_SIZE, VOID_SIZE, "nb-enemies").setTextAlign(TextAlign::RIGHT);
		allUI.speedImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/speed.png");
		allUI.speedImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/speed_active.png");
		allUI.speedText = &addText(VOID_SIZE, VOID_SIZE, "speed").setTextAlign(TextAlign::RIGHT);
		allUI.bonusBombImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/bomb.png");
		allUI.bonusBombImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/bomb_active.png");
		allUI.bonusBombText = &addText(VOID_SIZE, VOID_SIZE, "total-bombs").setTextAlign(TextAlign::RIGHT);
		allUI.bonusFlameImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/flame.png");
		allUI.bonusFlameImgActive = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/flame_active.png");
		allUI.bonusFlameText = &addText(VOID_SIZE, VOID_SIZE, "bomb-propagation").setTextAlign(TextAlign::RIGHT);
		allUI.bonusFlampassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/flampass.png");
		allUI.bonusFlampassImgActive = &addImage(VOID_SIZE, VOID_SIZE,
			"bomberman-assets/textures/bonus/flampass_active.png");
		allUI.bonusWallpassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/wallpass.png");
		allUI.bonusWallpassImgActive = &addImage(VOID_SIZE, VOID_SIZE,
			"bomberman-assets/textures/bonus/wallpass_active.png");
		allUI.bonusDetonatorImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/detonator.png");
		allUI.bonusDetonatorImgActive = &addImage(VOID_SIZE, VOID_SIZE,
			"bomberman-assets/textures/bonus/detonator_active.png");
		allUI.bonusBombpassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/bombpass.png");
		allUI.bonusBombpassImgActive = &addImage(VOID_SIZE, VOID_SIZE,
			"bomberman-assets/textures/bonus/bombpass_active.png");
		allUI.bonusShieldImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/shield.png");
		allUI.bonusShieldImgActive = &addImage(VOID_SIZE, VOID_SIZE,
			"bomberman-assets/textures/bonus/shield_active.png");
		allUI.bonusShieldText = &addText(VOID_SIZE, VOID_SIZE, "invulnerable").setTextAlign(TextAlign::RIGHT);
	} catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief LoadGameInformations
 */
void		SceneGame::_loadGameInfos() {
	try {
		allUI.timeLeftImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.timeLeftImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.timeLeftText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.scoreImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.scoreImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.scoreText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.lifeImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.lifeImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.lifeText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.levelNameText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.enemiesCounterText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.speedImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.speedImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.speedText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlameImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlameImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlameText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlampassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlampassImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusWallpassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusWallpassImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusDetonatorImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusDetonatorImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombpassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombpassImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusShieldImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusShieldImgActive->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusShieldText->setPos(VOID_SIZE).setSize(VOID_SIZE);
	} catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief Init the bonus member according to the description in level.json
 *
 * @return true if success
 * @return false if error.
 */
bool			SceneGame::_initBonus() {
	try {
		if (bonus.size())
			bonus.erase(bonus.begin(), bonus.end());
		SettingsJson	&lvl = getSettingsLevel();
		for (auto &&pair : Bonus::bonus) {
			bonus.insert({
				pair.first,
				{
					lvl.j("bonus").j(pair.first).i("chance"),
					lvl.j("bonus").j(pair.first).i("nb")
				}
			});
		}
	} catch (SceneGameException &e) {
		logErr(e.what());
		return false;
	}
	return true;
}
