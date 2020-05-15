#include "Bonus.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Static members -----------------------------------------------------------

std::map<BonusType::Enum, Block::Enum> Bonus::_textures = {
	{ BonusType::LIFE, Block::BONUS_LIFE },
	{ BonusType::BOMBS, Block::BONUS_BOMBS },
	{ BonusType::FLAMES, Block::BONUS_FLAMES },
	{ BonusType::SPEED, Block::BONUS_SPEED },
	{ BonusType::WALLPASS, Block::BONUS_WALLPASS },
	{ BonusType::DETONATOR, Block::BONUS_DETONATOR },
	{ BonusType::BOMBPASS, Block::BONUS_BOMBPASS },
	{ BonusType::FLAMPASS, Block::BONUS_FLAMPASS },
	{ BonusType::SHIELD, Block::BONUS_SHIELD },
	{ BonusType::TIME, Block::BONUS_TIME },
	{ BonusType::POINTS, Block::BONUS_POINTS },
};

std::unordered_map<std::string, BonusType::Enum> Bonus::bonus = {
	{ "life", BonusType::LIFE },
	{ "bombs", BonusType::BOMBS },
	{ "flames", BonusType::FLAMES },
	{ "speed", BonusType::SPEED },
	{ "wallpass", BonusType::WALLPASS },
	{ "detonator", BonusType::DETONATOR },
	{ "bombpass", BonusType::BOMBPASS },
	{ "flampass", BonusType::FLAMPASS },
	{ "shield", BonusType::SHIELD },
	{ "time", BonusType::TIME },
	{ "points", BonusType::POINTS },
};

std::unordered_map<BonusType::Enum, std::string> Bonus::description = {
	{ BonusType::LIFE, "Bonus Life: You earn an extra life." },
	{ BonusType::BOMBS, "Bonus Bombs: You can put one more bomb simultaneously." },
	{ BonusType::FLAMES, "Bonus Flames: The bombs explode at a greater range." },
	{ BonusType::SPEED, "Bonus Speed: You move faster." },
	{ BonusType::WALLPASS, "Bonus Wall Pass: You can pass through cracked walls." },
	{ BonusType::DETONATOR, DETONATOR_DESC },  // updated in settings
	{ BonusType::BOMBPASS, "Bonus Bomb Pass: You can now walk over bombs" },
	{ BonusType::FLAMPASS, "Bonus Flame Pass: You are not affected by bomb anymore." },
	{ BonusType::SHIELD, "Bonus Shield: You can't get damage for a while." },
	{ BonusType::TIME, "Bonus Time: Extra time to finish the level." },
	{ BonusType::POINTS, "Bonus Points: Your total score increase." },
};

std::unordered_map<BonusType::Enum, std::string> Bonus::bonusTextures = {
	{ BonusType::LIFE, "bomberman-assets/3dModels/bonus/textures/life.png" },
	{ BonusType::BOMBS, "bomberman-assets/3dModels/bonus/textures/bomb.png" },
	{ BonusType::FLAMES, "bomberman-assets/3dModels/bonus/textures/flamme.png" },
	{ BonusType::SPEED, "bomberman-assets/3dModels/bonus/textures/speed.png" },
	{ BonusType::WALLPASS, "bomberman-assets/3dModels/bonus/textures/wallpass.png" },
	{ BonusType::DETONATOR, "bomberman-assets/3dModels/bonus/textures/detonator.png" },
	{ BonusType::BOMBPASS, "bomberman-assets/3dModels/bonus/textures/bombpass.png" },
	{ BonusType::FLAMPASS, "bomberman-assets/3dModels/bonus/textures/flampass.png" },
	{ BonusType::SHIELD, "bomberman-assets/3dModels/bonus/textures/shield.png" },
	{ BonusType::TIME, "bomberman-assets/3dModels/bonus/textures/time.png" },
	{ BonusType::POINTS, "bomberman-assets/3dModels/bonus/textures/score.png" },
};

// -- Constructors -------------------------------------------------------------

Bonus::Bonus(SceneGame &game) : AObject(game) {
	type = Type::BONUS;
	name = "Bonus";
	blockPropagation = false;
	destructible = false;
	_indestructible = 0.5f;
	_toDraw = 0;
	_timeToDie = 10.0f;
	_typeBonus = _pickBonus();
	AudioManager::loadSound(BONUS_SOUND);
	AudioManager::loadSound(NEW_LIFE_SOUND);
	AudioManager::loadSound(BONUS_DISAPPEAR_SOUND);
}

Bonus::~Bonus() {
	game.clearFromBoard(this, {position.x, position.z});
}

Bonus::Bonus(Bonus const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Bonus &Bonus::operator=(Bonus const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
		_typeBonus = rhs._typeBonus;
		_toDraw = rhs._toDraw;
		_indestructible = rhs._indestructible;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Bonus::update() {
	if (!alive || !active)
		return true;

	if (_indestructible > 0)
		_indestructible -= game.getDtTime();
	else if (!destructible)
		destructible = true;

	_timeToDie -= game.getDtTime();
	if (_timeToDie <= 0.0) {
		try {
			AudioManager::playSound(BONUS_DISAPPEAR_SOUND);
		} catch(Sound::SoundException const & e) {
			logErr(e.what());
		}
		alive = false;
	}

	if (game.player->hasCollision(position)) {
		game.player->takeBonus(_typeBonus);
		if (_typeBonus == BonusType::LIFE) {
			try {
				AudioManager::playSound(NEW_LIFE_SOUND);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
		} else {
			try {
				AudioManager::playSound(BONUS_SOUND);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
		}
		active = false;
	}

	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	Bonus::postUpdate() {
	if (!alive || !active) {
		delete this;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Bonus::draw(Gui &gui) {
	(void)gui;

	// flash before dying
	if (_timeToDie <= 2) {
		_toDraw = ((_toDraw + 1) % 10);
		if (_toDraw > 5)
			return true;
	}

	// gui.drawCube(_textures[_typeBonus], getPos());

	// draw model
	try {
		_model->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	return true;
}

/**
 * @brief Get the description of a given bonus
 *
 * @param type The bonus type
 * @return std::string The description
 */
std::string	Bonus::getDescription(BonusType::Enum type) {
	auto it = description.find(type);
	if (it != description.end()) {
		return it->second;
	}
	logErr("Bonus::getDescription: Invalid bonus type " << type);
	return "ERROR";
}

/**
 * @brief Static class to generate bonus.
 *
 * @param game SceneGame object.
 * @param rate Probability to generate an bonus (between 0 and 1). default: 0.1f
 * @return Bonus* or nullptr if nothing has been generated.
 */
Bonus	*Bonus::generateBonus(SceneGame &game, float rate) {
	if (rate <= 0.0f)
		return nullptr;

	int32_t	totalChances = 0;
	for (auto &&it : game.bonus) {
		if (it.second.nb > 0 || it.second.nb == -1)
			totalChances += it.second.chance;
	}

	if (totalChances == 0)
		return nullptr;

	if (rate >= 1.0f)
		return new Bonus(game);

	int		percentRate = rand() % 100;
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;

	return new Bonus(game);
}

// -- Privates methods ---------------------------------------------------------

/**
 * @brief pick randomly type of bonus according to his chance.
 *
 * @return BonusType::Enum bonus type
 */
BonusType::Enum		Bonus::_pickBonus() {
	int32_t	totalChances = 0;

	for (auto &&it : game.bonus) {
		if (it.second.nb > 0 || it.second.nb == -1)
			totalChances += it.second.chance;
	}
	int32_t	n = ((static_cast<double>(rand()) / (RAND_MAX)) + 1) * totalChances;
	while (true) {
		for (auto &&it : game.bonus) {
			if (it.second.nb == 0)
				continue;
			n -= it.second.chance;
			if (n < 0) {
				if (it.second.nb > 0)
					it.second.nb--;
				return bonus[it.first];
			}
		}
	}
}

/**
 * @brief Init Bonus
 *
 * @return true on success
 * @return false on failure
 */
bool	Bonus::init() {
	AObject::init();

	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		_model = new Model(ModelsManager::getModel("bonus"), game.getDtTime(),
			ETransform((position + glm::vec3(.5, 0, .5))));
		_model->play = true;
		_model->loopAnimation = true;
		_model->setAnimation("Armature|idle");
		_model->setMeshTexture(TextureType::DIFFUSE, "Bonus_Mesh::Bonus",
			bonusTextures[_typeBonus]);
	}
	catch(ModelsManager::ModelsManagerException const &e) {
		logErr(e.what());
		return false;
	}
	catch(OpenGLModel::ModelException const &e) {
		logErr(e.what());
		return false;
	}

	return true;
}

// -- Exceptions errors --------------------------------------------------------

Bonus::BonusException::BonusException()
: std::runtime_error("Bonus Exception") {}

Bonus::BonusException::BonusException(const char* whatArg)
: std::runtime_error(std::string(std::string("BonusError: ") + whatArg).c_str()) {}
